# OrderFlow: Trading Order Book Simulator

A console-based **stock exchange order book simulator** written in **C++17**, built as a Data Structures and Algorithms mini project.

Users place BUY and SELL limit orders for one fictional stock. OrderFlow matches them using **price-time priority**, records every trade, and keeps unmatched orders resting in the book.

> Educational project only. There is no real market connection and no real money involved.

---

## Table of Contents

- [Features](#features)
- [How Matching Works](#how-matching-works)
- [Data Structures Used](#data-structures-used)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Sample Run](#sample-run)
- [Time Complexity](#time-complexity)
- [Future Improvements](#future-improvements)
- [Team](#team)

---

## Features

- Submit **BUY** and **SELL** limit orders
- Automatic matching with **price-time priority**
- **Partial fills**: leftover quantity rests in the book
- **Cancel** any active order by its ID
- View the live **order book**, full **trade history** and a **market summary** (best bid, best ask, spread, volume, volume-weighted average price)
- Built-in **demo scenario** for quick demonstrations
- Input validation: rejects zero or negative prices and quantities, and handles non-numeric input without crashing

## How Matching Works

1. **Best price first:** the highest buy price (best bid) and the lowest sell price (best ask) are matched first.
2. **Earlier order first:** at the same price, the order that arrived first is matched first (FIFO).
3. **Trade price:** a trade executes at the price of the order that was already waiting in the book.
4. **Leftover quantity:** any unfilled quantity rests in the book until it is matched or cancelled.

A new BUY matches when its price is **greater than or equal to** the best ask. A new SELL matches when its price is **less than or equal to** the best bid.

## Data Structures Used

OrderFlow uses **4 STL containers** (`map`, `list`, `unordered_map`, `vector`) in **5 places**, because the `map` is used once for each side of the book.

| Container | Variable | Stores | Why it is used |
|---|---|---|---|
| `map<int, OrderList, greater<int>>` | `buyLevels` | price to list of buy orders | Prices stay sorted high to low, so the best bid is always `begin()` |
| `map<int, OrderList>` | `sellLevels` | price to list of sell orders | Prices stay sorted low to high, so the best ask is always `begin()` |
| `list<Order>` | `OrderList` (one per price) | orders at one price in arrival order | FIFO queue; erasing by iterator is O(1) and other iterators stay valid |
| `unordered_map<int, OrderLocation>` | `activeOrders` | order ID to its location in the book | Finds an order by ID in O(1) average, which makes cancel fast |
| `vector<Trade>` | `tradeHistory` | all executed trades in order | Trades are only appended and read in order |

User-defined types: `struct Order`, `struct Trade`, `enum class Side`.

## Project Structure

```
orderflow/
├── main.cpp       # Entry point: menu loop that calls the other files
├── Types.h        # Side, Order, Trade
├── OrderBook.h    # Core logic: data structures, matching, cancel
├── Display.h      # Order book, trade history and market summary output
├── Menu.h         # Input helpers, menu text, demo scenario
└── README.md
```

| File | Responsibility |
|---|---|
| `Types.h` | Basic types shared by every other file |
| `OrderBook.h` | The `OrderBook` class: owns all data structures and all exchange rules |
| `Display.h` | Read-only printing functions; they cannot modify the book |
| `Menu.h` | Safe integer input, banner, menu and the demo scenario |
| `main.cpp` | Creates the order book and runs the menu loop |

## Getting Started

### Requirements

- A C++17 compiler (g++ 7 or newer, clang, or MSVC)

### Build and run

```bash
git clone https://github.com/<your-username>/<your-repo>.git
cd <your-repo>

g++ -std=c++17 main.cpp -o orderflow
./orderflow
```

On Windows (MinGW), run `orderflow.exe` instead of `./orderflow`.

### Menu

```
1. Submit BUY limit order
2. Submit SELL limit order
3. Cancel an active order
4. Display current order book
5. Display trade history
6. Display market summary
7. Load demo scenario
0. Exit
```

Prices are whole rupees and quantities are whole shares.

## Sample Run

Choosing option **7** submits three orders: BUY 10 at 100, SELL 6 at 98, SELL 8 at 100.

```
Accepted order #1: BUY 10 share(s) at Rs. 100
Order #1 has 10 share(s) remaining and is now resting in the book.

Accepted order #2: SELL 6 share(s) at Rs. 98
  TRADE EXECUTED | Trade #1 | Buy order #1 | Sell order #2 | Quantity: 6 | Price: Rs. 100
Order #2 was completely filled.

Accepted order #3: SELL 8 share(s) at Rs. 100
  TRADE EXECUTED | Trade #2 | Buy order #1 | Sell order #3 | Quantity: 4 | Price: Rs. 100
Order #3 has 4 share(s) remaining and is now resting in the book.
```

Option **4** (order book):

```
================ ORDER BOOK ================
SELL ORDERS (best / lowest ask first)
Price       Order ID      Remaining Qty   Sequence
----------------------------------------------------
100         3             4               3
---------------------------------------------
BUY ORDERS (best / highest bid first)
  No active buy orders.
=============================================
```

Option **6** (market summary):

```
================ MARKET SUMMARY ================
Best bid: None
Best ask: Rs. 100 | Quantity at best ask: 4
Bid-ask spread: Not available (one side of the book is empty).
Active orders: 1
Trades executed: 2
Total shares traded: 10
Total traded value: Rs. 1000
Volume-weighted average execution price: Rs. 100.00
Latest execution: 4 share(s) at Rs. 100
================================================
```

## Time Complexity

L is the number of price levels and n is the number of active orders.

| Operation | Cost | Reason |
|---|---|---|
| Find best bid or best ask | O(1) | `begin()` of the map |
| Add an order at a price level | O(log L) | map lookup or insert, then list `push_back` is O(1) |
| Match one waiting order | O(1) | front of the list; erasing an emptied level costs O(log L) |
| Cancel an order | O(log L) | ID lookup O(1) average, list erase O(1), finding the level O(log L) |
| Record a trade | O(1) amortized | vector `push_back` |
| Display book or history | O(n) | one pass through the data |

## Future Improvements

- Support for multiple stocks
- Market orders
- Saving and loading the order book from a file
- Splitting the headers into `.h` and `.cpp` pairs for a larger codebase

## Team

| Name | Roll No. |
|---|---|
| [Member 1] | [Roll No.] |
| [Member 2] | [Roll No.] |
| [Member 3] | [Roll No.] |

**Guide:** [Guide Name]
**College:** [College Name]
**Department:** [Department Name]
