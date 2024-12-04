#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>


using json = nlohmann::json;

// Function to handle the response from the cURL request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// General function to send a cURL request with optional access token
std::string sendRequest(const std::string& url, const json& payload, const std::string& accessToken = "") {
    std::string readBuffer;

    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Convert payload to string and set it as the POST fields
        std::string jsonStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!accessToken.empty()) {
            std::string authHeader = "Authorization: Bearer " + accessToken;
            headers = curl_slist_append(headers, authHeader.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set SSL verification to bypass for debugging
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Set up the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
        }

        // Print raw response for debugging
        std::cout << "Raw Response: " << readBuffer << std::endl;

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();

    return readBuffer;
}

// Function to get the access token
std::string getAccessToken(const std::string& clientId, const std::string& clientSecret) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 0},
        {"method", "public/auth"},
        {"params", {
            {"grant_type", "client_credentials"},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        }}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/auth", payload);
    if (response.empty()) {
        std::cerr << "Failed to receive a response or response is empty." << std::endl;
        return "";
    }

    auto responseJson = json::parse(response, nullptr, false);
    if (responseJson.is_discarded()) {
        std::cerr << "Failed to parse JSON response." << std::endl;
        return "";
    }

    // Retrieve the access token from the response
    if (responseJson.contains("result") && responseJson["result"].contains("access_token")) {
        return responseJson["result"]["access_token"];
    } else {
        std::cerr << "Failed to retrieve access token." << std::endl;
        return "";
    }
}

// Function to place an order
void placeOrder(const std::string& orderType, const std::string& price, const std::string& accessToken, const std::string& amount, const std::string& instrument) {
    if (orderType != "buy" && orderType != "sell") {
        std::cerr << "Invalid order type. Must be 'buy' or 'sell'." << std::endl;
        return;
    }

    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", std::string("private/") + orderType},
        {"params", {
            {"instrument_name", instrument},
            {"amount", amount},
            {"price", price},
            {"post_only", true}
        }}
    };

    std::string endpoint = "https://test.deribit.com/api/v2/private/" + orderType;
    std::string response = sendRequest(endpoint, payload, accessToken);

    if (response.empty()) {
        std::cerr << "Failed to receive a response or response is empty." << std::endl;
        return;
    }

    auto responseJson = json::parse(response, nullptr, false);
    if (responseJson.is_discarded()) {
        std::cerr << "Failed to parse JSON response." << std::endl;
        return;
    }

    if (responseJson.contains("result")) {
        std::cout << "Order placed successfully: " << responseJson["result"] << std::endl;
    } else {
        std::cerr << "Failed to place order." << std::endl;
    }
}


void cancelOrder(const std::string& orderId, const std::string& accessToken) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 2},
        {"method", "private/cancel"},
        {"params", {
            {"order_id", orderId}
        }}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/cancel", payload, accessToken);
    auto responseJson = json::parse(response, nullptr, false);

    if (responseJson.contains("result")) {
        std::cout << "Order canceled successfully: " << responseJson["result"] << std::endl;
    } else {
        std::cerr << "Failed to cancel order." << std::endl;
    }
}

void modifyOrder(const std::string& orderId, const std::string& newPrice, const std::string& accessToken,int amount) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 3},
        {"method", "private/edit"},
        {"params", {
            {"order_id", orderId},
            {"amount",amount},
            {"price", newPrice}
        }}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/edit", payload, accessToken);
    auto responseJson = json::parse(response, nullptr, false);

    if (responseJson.contains("result")) {
        std::cout << "Order modified successfully: " << responseJson["result"] << std::endl;
    } else {
        std::cerr << "Failed to modify order." << std::endl;
    }
}

void getOrderBook(const std::string& instrument, const std::string& accessToken) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 4},
        {"method", "public/get_order_book"},
        {"params", {
            {"instrument_name", instrument}
        }}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/get_order_book", payload, accessToken);
    auto responseJson = json::parse(response, nullptr, false);

    if (responseJson.contains("result")) {
        auto result = responseJson["result"];
        std::cout << "=== Order Book for " << instrument << " ===\n";

        std::cout << "Best Bids:\n";
        for (const auto& bid : result["bids"]) {
            std::cout << "Price: " << bid[0] << ", Quantity: " << bid[1] << '\n';
        }

        std::cout << "\nBest Asks:\n";
        for (const auto& ask : result["asks"]) {
            std::cout << "Price: " << ask[0] << ", Quantity: " << ask[1] << '\n';
        }

        std::cout << "\nLast Price: " << result["last_price"] << "\n";
    } else {
        std::cerr << "Failed to retrieve order book." << std::endl;
    }
}

void viewCurrentPosition(const std::string& accessToken, const std::string& instrument) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 5},
        {"method", "private/get_positions"},
        {"params", {
            {"instrument_name", instrument}
        }}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/get_positions", payload, accessToken);
    auto responseJson = json::parse(response, nullptr, false);

    if (responseJson.contains("result")) {
        auto positions = responseJson["result"];
        std::cout << "=== Current Positions for " << instrument << " ===\n";

        for (const auto& position : positions) {
            std::cout << "Instrument Name: " << position["instrument_name"] << '\n';
            std::cout << "Size currency: " << position["size_currency"] << '\n';
            std::cout << "Average Price: " << position["average_price"] << '\n';
            std::cout << "Mark Price: " << position["mark_price"] << '\n';

            std::cout << "-------------------------------\n";
        }
    } else {
        std::cerr << "Failed to retrieve current positions." << std::endl;
    }
}

void getOpenOrders(const std::string& accessToken) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 6},
        {"method", "private/get_open_orders"},
        {"params", {{"kind","future"},{"type","limit"}}},
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/get_open_orders", payload, accessToken);
    auto responseJson = json::parse(response, nullptr, false);

    if (responseJson.contains("result")) {
        auto orders = responseJson["result"];
        if (orders.empty()) {
            std::cout << "No open orders found.\n";
            return;
        }

        std::cout << "=== Open Orders ===\n";
        for (const auto& order : orders) {
            std::cout << "Order ID: " << order["order_id"] << '\n';
            std::cout << "Instrument Name: " << order["instrument_name"] << '\n';
            std::cout << "Price: " << order["price"] << '\n';
            std::cout << "Amount: " << order["amount"] << '\n';
            std::cout << "Direction: " << order["direction"] << '\n';
            std::cout << "Order State: " << order["order_state"] << '\n';
            std::cout << "Post Only: " << (order["post_only"].get<bool>() ? "Yes" : "No") << '\n';
            std::cout << "Time In Force: " << order["time_in_force"] << '\n';
            std::cout << "-------------------------------\n";
        }
    } else {
        std::cerr << "Failed to retrieve open orders." << std::endl;
    }
}



int main() {

    const std::string clientId = "QpeDqV6n";
    const std::string clientSecret = "DyGxsCrOAYzP22iPcXLiu3pcmNq2Z9NbPfg-egQ6cL4";

    std::string accessToken = getAccessToken(clientId, clientSecret);
    if (!accessToken.empty()) 
    {
        //placeOrder("buy","1000", accessToken, "1000", "BTC-5AUG23");
        //placeOrder("buy","10", accessToken, "10", "BTC-PERPETUAL");
        //placeOrder("buy","50", accessToken, "50", "BTC-PERPETUAL");
        //placeOrder("sell","20", accessToken, "20", "ETH-PERPETUAL");

        //getOpenOrders(accessToken);
        //cancelOrder("30454002197",accessToken);
        //modifyOrder("30453961880","500",accessToken,500);
        //getOrderBook("ETH-PERPETUAL",accessToken);
        //viewCurrentPosition(accessToken,"ETH-PERPETUAL");
    }
    
    return 0;

}
