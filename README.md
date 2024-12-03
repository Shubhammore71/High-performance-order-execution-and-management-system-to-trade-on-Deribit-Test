# High-Performance Trading and WebSocket Server System

## Overview
This project demonstrates the development of a high-performance order execution and management system integrated with Deribit Testnet API for cryptocurrency trading. Additionally, it features a WebSocket server for real-time market data streaming, ensuring seamless interaction between clients and trading systems.

## Key Features

### **1. Order Execution and Management**
- **Place Orders:** Buy or sell orders with customizable parameters such as instrument, price, and amount.
- **Cancel Orders:** Efficiently cancel specific orders by their ID.
- **Modify Orders:** Edit existing orders to adjust price and amount dynamically.
- **Retrieve Open Orders:** Fetch a list of all active orders.

### **2. Market Data Management**
- **Order Book Retrieval:** Access real-time order book data for any trading instrument.
- **View Positions:** Display current positions with detailed metrics like size, average price, and mark price.

### **3. WebSocket Server**
- **Client Connectivity:** Supports multiple client connections using Boost.Asio and Boost.Beast.
- **Dynamic Subscriptions:** Allows clients to subscribe to specific symbols for order book updates.
- **Real-Time Broadcasting:** Sends live updates to subscribed clients for order book and market changes.

## Technology Stack
- **C++** for core implementation.
- **Boost.Asio and Boost.Beast** for WebSocket server functionality.
- **cURL** for HTTP requests to the Deribit API.
- **nlohmann/json** for JSON parsing and handling.

## Project Structure
```
├── main.cpp
─ websocket/          # Main application logic for order management and API integration.
├── server.h          # Header file for WebSocket server class.
├── server.cpp        # Implementation of WebSocket server functions.

```

## Getting Started

### Prerequisites
- **Compiler:** A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
- **Libraries:**
  - Boost (Asio and Beast)
  - cURL
  - nlohmann/json
- **API Credentials:** Obtain `client_id` and `client_secret` from Deribit Testnet.

### Building the Project
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-folder>
   ```
2. Compile the code:
   ```bash
   g++ -std=c++17 main.cpp server.cpp -o trading_system -lboost_system -lssl -lcrypto -lcurl
   ```

### Running the Project
1. Execute the compiled binary:
   ```bash
   ./trading_system
   ```
2. The WebSocket server will run on the specified port, and trading operations will execute based on the implemented logic in `main.cpp`.

## How It Works

### **1. API Integration**
- Authenticate with Deribit Testnet to retrieve an access token.
- Use the access token to send authenticated requests for order management and data retrieval.

### **2. WebSocket Server**
- Listens for incoming client connections on a specified port.
- Handles client subscriptions to specific symbols for real-time data updates.
- Broadcasts live market data changes to all subscribed clients.

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
- Server response: `Acknowledged: subscribe:BTC-PERPETUAL`

## Future Enhancements
- Add error handling and retry mechanisms for network interruptions.
- Extend WebSocket server to support additional subscription types (e.g., trades, positions).
- Implement multi-threading for higher performance and scalability.

## Contact
For any inquiries or contributions, please reach out to:
- **Email:** [shubham.more@iitgn.ac.in]
- **GitHub:** [your-github-profile]

---
This project showcases a robust and efficient trading system with real-time WebSocket capabilities, highlighting advanced C++ skills and expertise in API integration. The modular design and clear documentation ensure scalability and ease of understanding.

