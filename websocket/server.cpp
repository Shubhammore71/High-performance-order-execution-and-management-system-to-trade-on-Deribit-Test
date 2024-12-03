#include "server.h"
#include <iostream>

using namespace boost::asio;
using namespace boost::beast;


WebSocketServer::WebSocketServer(unsigned short port)
    : port(port), acceptor(ioContext, ip::tcp::endpoint(ip::tcp::v4(), port)) {}


void WebSocketServer::run() {
    std::cout << "WebSocket server is running on port " << port << std::endl;
    acceptConnection();
    ioContext.run();
}


void WebSocketServer::acceptConnection() {
    auto socket = std::make_shared<ip::tcp::socket>(ioContext);
    acceptor.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
            auto ws = std::make_shared<websocket::stream<ip::tcp::socket>>(std::move(*socket));
            handleConnection(ws);
        } else {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }
        acceptConnection(); 
    });
}


void WebSocketServer::handleConnection(std::shared_ptr<websocket::stream<ip::tcp::socket>> ws) {
    try {
        ws->accept();
        std::cout << "Client connected!" << std::endl;

        while (true) {
            flat_buffer buffer;
            ws->read(buffer);
            std::string message = buffers_to_string(buffer.data());
            std::cout << "Received: " << message << std::endl;

            // Example: Handle subscription requests dynamically
            if (message.find("subscribe:") == 0) {
                std::string symbol = message.substr(10);
                std::cout << "Subscribing client to: " << symbol << std::endl;

                // Add subscription logic for the symbol
            }

            // Reply to the client
            ws->write(net::buffer("Acknowledged: " + message));
        }
    } catch (const std::exception& e) {
        std::cerr << "WebSocket error: " << e.what() << std::endl;
    }
}


void WebSocketServer::subscribe(const std::string& symbol, std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws) {
    auto it = subscribers.find(symbol);
    if (it == subscribers.end()) {
        subscribers[symbol] = std::vector<std::shared_ptr<websocket::stream<ip::tcp::socket>>>();
    }
    subscribers[symbol].push_back(ws);
    std::cout << "Client subscribed to symbol: " << symbol << std::endl;
}


void WebSocketServer::unsubscribe(const std::string& symbol, std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws) {
    auto it = subscribers.find(symbol);
    if (it != subscribers.end()) {
        auto& wsList = it->second;
        wsList.erase(std::remove(wsList.begin(), wsList.end(), ws), wsList.end());

        if (wsList.empty()) {
            subscribers.erase(it);
        }
        std::cout << "Client unsubscribed from symbol: " << symbol << std::endl;
    }
}


void WebSocketServer::broadcastOrderBookUpdates(const std::string& symbol, const std::string& orderBook) {
    std::cout << "Broadcasting updates for symbol: " << symbol << " with data: " << orderBook << std::endl;
    auto it = subscribers.find(symbol);
    if (it != subscribers.end()) {
        for (auto& ws : it->second) {
            try {
                ws->write(net::buffer(orderBook));
            } catch (const std::exception& e) {
                std::cerr << "Error broadcasting to a subscriber: " << e.what() << std::endl;
            }
        }
    }
}
