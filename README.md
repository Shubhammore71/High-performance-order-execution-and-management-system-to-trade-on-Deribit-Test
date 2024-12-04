# High-Performance Trading and WebSocket Server System

## Overview
This project is a high-performance order execution and management system built to trade on the Deribit Testnet API. It includes a robust WebSocket server for real-time market data streaming, enabling seamless interaction between trading clients and the system.

## Key Features

### **Order Execution and Management**
- **Place Orders:** Submit buy or sell orders with configurable parameters like instrument, price, and amount.
- **Cancel Orders:** Efficiently cancel specific orders by their unique ID.
- **Modify Orders:** Dynamically update existing orders for price and quantity adjustments.
- **Retrieve Open Orders:** Fetch details of all active orders.

### **Market Data Management**
- **Order Book Retrieval:** Access real-time order book data for any trading instrument.
- **View Positions:** Display detailed information on current positions, including size, average price, and mark price.

### **WebSocket Server**
- **Client Connectivity:** Supports multiple client connections via Boost.Asio and Boost.Beast.
- **Dynamic Subscriptions:** Allows clients to subscribe to specific symbols for real-time order book updates.
- **Real-Time Broadcasting:** Continuously streams live updates for subscribed symbols to all connected clients.

## Technology Stack
- **C++**: Core implementation language.
- **Boost.Asio and Boost.Beast**: For WebSocket server functionality.
- **cURL**: For making HTTP requests to the Deribit API.
- **nlohmann/json**: For JSON parsing and manipulation.

## Project Structure
```
project_root/
├── main.cpp          # Entry point for the application.
├── websocket/        # Contains WebSocket server logic.
│   ├── server.h      # Header file for WebSocket server class.
│   └── server.cpp    # Implementation of WebSocket server functions.
```

## Getting Started

### Prerequisites
- **Compiler:** A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
- **Libraries:**
  - Boost (Asio and Beast)
  - cURL
  - nlohmann/json
- **API Credentials:** Generate `client_id` and `client_secret` from the [Deribit Testnet](https://test.deribit.com/).

### Building the Project
1. Clone the repository:
   ```bash
   git clone https://github.com/Shubhammore71/deribit-trader-cpp
   cd deribit-trader-cpp
   ```
2. Compile the code:
   ```bash
   g++ -std=c++17 main.cpp websocket/server.cpp -o trading_system -lboost_system -lssl -lcrypto -lcurl
   ```

### Running the Project
1. Execute the compiled binary:
   ```bash
   ./trading_system
   ```
2. The WebSocket server will run on the specified port, and trading operations will execute based on the implemented logic in `main.cpp`.

## How It Works

### **1. API Integration**
- Authenticate with the Deribit Testnet API to retrieve an access token.
- Use the access token to send authenticated requests for order execution, cancellation, and data retrieval.

### **2. WebSocket Server**
- Listens for incoming connections and manages multiple clients.
- Handles subscriptions to specific trading symbols for order book updates.
- Broadcasts real-time market data updates to all connected clients based on their subscriptions.

## Example Usage

### Placing an Order
```cpp
placeOrder("buy", "100", accessToken, "1", "BTC-PERPETUAL");
```

### Cancelling an Order
```cpp
cancelOrder("30454002197", accessToken);
```

### Retrieving Order Book
```cpp
getOrderBook("ETH-PERPETUAL", accessToken);
```

### Subscribing to a Symbol (WebSocket Client)
- Send: `subscribe:BTC-PERPETUAL`
- Server Response: `Acknowledged: subscribe:BTC-PERPETUAL`

## Future Enhancements
- Enhance error handling and implement retry mechanisms for network failures.
- Extend WebSocket functionality to include additional data types like trades and positions.
- Optimize performance using multi-threading for handling high client traffic and lower latency.

## Contact
For inquiries or contributions:
- **Email:** [shubham.more@iitgn.ac.in](mailto:shubham.more@iitgn.ac.in)

---
This project showcases advanced C++ programming skills, expertise in WebSocket development, and proficiency in API integration. It is designed with modularity and scalability, making it a robust solution for real-time trading systems.

