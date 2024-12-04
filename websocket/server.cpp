#include "server.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
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
    auto buffer = std::make_shared<flat_buffer>(); // Use a shared buffer for async operations

    ws->async_read(*buffer, [this, ws, buffer](boost::system::error_code ec, std::size_t bytesTransferred) {
        if (!ec) {
            std::string message = buffers_to_string(buffer->data());
            std::cout << "Received: " << message << std::endl;

            // Handle message
            if (message.find("subscribe:") == 0) {
                std::string symbol = message.substr(10);
                std::cout << "Subscribing client to: " << symbol << std::endl;
                subscribe(symbol, ws);
            }

            // Respond to the client
            ws->async_write(net::buffer("Acknowledged: " + message), [](boost::system::error_code writeEc, std::size_t) {
                if (writeEc) {
                    std::cerr << "Write error: " << writeEc.message() << std::endl;
                }
            });

            // Continue reading
            handleConnection(ws);
        } else {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
    });
}

void WebSocketServer::subscribe(const std::string& symbol, std::shared_ptr<websocket::stream<ip::tcp::socket>> ws) {
    std::lock_guard<std::mutex> lock(subscribersMutex);
    subscribers[symbol].push_back(ws);
    std::cout << "Subscribed to: " << symbol << std::endl;
}

void WebSocketServer::unsubscribe(const std::string& symbol, std::shared_ptr<websocket::stream<ip::tcp::socket>> ws) {
    std::lock_guard<std::mutex> lock(subscribersMutex);
    auto& vec = subscribers[symbol];
    vec.erase(std::remove(vec.begin(), vec.end(), ws), vec.end());
    if (vec.empty()) {
        subscribers.erase(symbol);
    }
}

void WebSocketServer::broadcastOrderBookUpdates(const std::string& symbol, const std::string& orderBook) {
    std::lock_guard<std::mutex> lock(subscribersMutex);
    auto it = subscribers.find(symbol);
    if (it != subscribers.end()) {
        for (auto& ws : it->second) {
            if (ws->is_open()) {
                try {
                    ws->write(net::buffer(orderBook));
                } catch (const std::exception& e) {
                    std::cerr << "Broadcast error: " << e.what() << std::endl;
                }
            }
        }
    }
}
