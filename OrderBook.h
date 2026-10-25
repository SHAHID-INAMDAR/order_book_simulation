// OrderBook.h - core logic: data structures, order matching, cancel
#pragma once

#include <iostream>
#include <map>
#include <list>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "Types.h"

class OrderBook {
public:
    // At each price, orders are stored in a list in arrival order (FIFO).
    using OrderList = list<Order>;

    // Buy prices are sorted from highest to lowest.
    using BuyLevels = map<int, OrderList, greater<int>>;

    // Sell prices are sorted from lowest to highest.
    using SellLevels = map<int, OrderList>;

private:
    BuyLevels buyLevels;
    SellLevels sellLevels;

    // Lets us find an active order quickly by its ID.
    // The iterator points to the order's node inside its price-level list.
    struct OrderLocation {
        Side side;
        int price;
        OrderList::iterator position;
    };

    unordered_map<int, OrderLocation> activeOrders;
    vector<Trade> tradeHistory;

    int nextOrderId = 1;
    long long nextSequence = 1;
    long long totalSharesTraded = 0;
    long long totalTradedValue = 0;

    // Add a remaining order to the correct side of the book.
    void addToBook(const Order& order) {
        if (order.side == Side::BUY) {
            OrderList& ordersAtPrice = buyLevels[order.price];
            ordersAtPrice.push_back(order);

            auto position = ordersAtPrice.end();
            --position; // Iterator to the order we just added.

            activeOrders.emplace(
                order.id,
                OrderLocation{order.side, order.price, position}
            );
        } else {
            OrderList& ordersAtPrice = sellLevels[order.price];
            ordersAtPrice.push_back(order);

            auto position = ordersAtPrice.end();
            --position;

            activeOrders.emplace(
                order.id,
                OrderLocation{order.side, order.price, position}
            );
        }
    }

    // Remove an active order from both the price-level list and ID index.
    // This is used after an order is completely filled and when cancelling.
    bool removeActiveOrder(int orderId) {
        auto locationIt = activeOrders.find(orderId);

        if (locationIt == activeOrders.end()) {
            return false;
        }

        Side side = locationIt->second.side;
        int price = locationIt->second.price;
        OrderList::iterator position = locationIt->second.position;

        if (side == Side::BUY) {
            auto levelIt = buyLevels.find(price);

            if (levelIt != buyLevels.end()) {
                levelIt->second.erase(position);

                // Remove an empty price level so the book stays clean.
                if (levelIt->second.empty()) {
                    buyLevels.erase(levelIt);
                }
            }
        } else {
            auto levelIt = sellLevels.find(price);

            if (levelIt != sellLevels.end()) {
                levelIt->second.erase(position);

                if (levelIt->second.empty()) {
                    sellLevels.erase(levelIt);
                }
            }
        }

        activeOrders.erase(locationIt);
        return true;
    }

    // Save and print one executed trade.
    void recordTrade(int buyOrderId, int sellOrderId, int price, int quantity) {
        Trade trade;
        trade.tradeId = static_cast<int>(tradeHistory.size()) + 1;
        trade.buyOrderId = buyOrderId;
        trade.sellOrderId = sellOrderId;
        trade.price = price;
        trade.quantity = quantity;

        tradeHistory.push_back(trade);
        totalSharesTraded += quantity;
        totalTradedValue += static_cast<long long>(price) * quantity;

        cout << "\n  TRADE EXECUTED"
             << " | Trade #" << trade.tradeId
             << " | Buy order #" << buyOrderId
             << " | Sell order #" << sellOrderId
             << " | Quantity: " << quantity
             << " | Price: Rs. " << price << "\n";
    }

    // Match a new BUY order against the best available SELL orders.
    void matchBuy(Order& incomingOrder) {
        while (incomingOrder.remainingQuantity > 0 && !sellLevels.empty()) {
            // The sell map is ascending, so begin() is the lowest ask.
            auto bestLevel = sellLevels.begin();
            int bestAsk = bestLevel->first;

            // A buy cannot pay above its limit price.
            if (incomingOrder.price < bestAsk) {
                break;
            }

            // First order at this price has time priority (FIFO).
            auto restingPosition = bestLevel->second.begin();
            int restingOrderId = restingPosition->id;
            int executionPrice = restingPosition->price;

            int tradeQuantity = min(
                incomingOrder.remainingQuantity,
                restingPosition->remainingQuantity
            );

            recordTrade(
                incomingOrder.id,
                restingOrderId,
                executionPrice,
                tradeQuantity
            );

            incomingOrder.remainingQuantity -= tradeQuantity;
            restingPosition->remainingQuantity -= tradeQuantity;

            // A fully filled resting order must be removed from the book.
            if (restingPosition->remainingQuantity == 0) {
                removeActiveOrder(restingOrderId);
            }
        }
    }

    // Match a new SELL order against the best available BUY orders.
    void matchSell(Order& incomingOrder) {
        while (incomingOrder.remainingQuantity > 0 && !buyLevels.empty()) {
            // The buy map uses greater<int>, so begin() is the highest bid.
            auto bestLevel = buyLevels.begin();
            int bestBid = bestLevel->first;

            // A sell cannot accept less than its limit price.
            if (incomingOrder.price > bestBid) {
                break;
            }

            auto restingPosition = bestLevel->second.begin();
            int restingOrderId = restingPosition->id;
            int executionPrice = restingPosition->price;

            int tradeQuantity = min(
                incomingOrder.remainingQuantity,
                restingPosition->remainingQuantity
            );

            // The resting order is a BUY and incoming order is a SELL.
            recordTrade(
                restingOrderId,
                incomingOrder.id,
                executionPrice,
                tradeQuantity
            );

            incomingOrder.remainingQuantity -= tradeQuantity;
            restingPosition->remainingQuantity -= tradeQuantity;

            if (restingPosition->remainingQuantity == 0) {
                removeActiveOrder(restingOrderId);
            }
        }
    }

public:
    // Accept an order, match it, then rest any unfilled quantity in the book.
    void submitOrder(Side side, int price, int quantity) {
        if (price <= 0) {
            cout << "Error: Price must be greater than zero.\n";
            return;
        }

        if (quantity <= 0) {
            cout << "Error: Quantity must be greater than zero.\n";
            return;
        }

        Order incomingOrder;
        incomingOrder.id = nextOrderId++;
        incomingOrder.side = side;
        incomingOrder.price = price;
        incomingOrder.remainingQuantity = quantity;
        incomingOrder.sequence = nextSequence++;

        cout << "\nAccepted order #" << incomingOrder.id
             << ": " << sideName(side)
             << " " << quantity
             << " share(s) at Rs. " << price << "\n";

        if (side == Side::BUY) {
            matchBuy(incomingOrder);
        } else {
            matchSell(incomingOrder);
        }

        if (incomingOrder.remainingQuantity > 0) {
            int remaining = incomingOrder.remainingQuantity;
            addToBook(incomingOrder);

            cout << "Order #" << incomingOrder.id
                 << " has " << remaining
                 << " share(s) remaining and is now resting in the book.\n";
        } else {
            cout << "Order #" << incomingOrder.id
                 << " was completely filled.\n";
        }
    }

    // Cancel only the unfilled quantity of an active order.
    void cancelOrder(int orderId) {
        auto locationIt = activeOrders.find(orderId);

        if (locationIt == activeOrders.end()) {
            cout << "Order #" << orderId
                 << " is not active (it may be filled, cancelled, or unknown).\n";
            return;
        }

        const Order& order = *(locationIt->second.position);
        cout << "Cancelling order #" << order.id
             << " | Side: " << sideName(order.side)
             << " | Price: Rs. " << order.price
             << " | Unfilled quantity cancelled: "
             << order.remainingQuantity << "\n";

        removeActiveOrder(orderId);
    }

    // Total unfilled quantity at the best price on one side.
    long long quantityAtTopOfBook(Side side) const {
        long long total = 0;

        if (side == Side::BUY) {
            if (!buyLevels.empty()) {
                for (const Order& order : buyLevels.begin()->second) {
                    total += order.remainingQuantity;
                }
            }
        } else {
            if (!sellLevels.empty()) {
                for (const Order& order : sellLevels.begin()->second) {
                    total += order.remainingQuantity;
                }
            }
        }

        return total;
    }

    // Read-only access used by the display functions in Display.h.
    const BuyLevels& getBuyLevels() const { return buyLevels; }
    const SellLevels& getSellLevels() const { return sellLevels; }
    const vector<Trade>& getTradeHistory() const { return tradeHistory; }
    size_t getActiveOrderCount() const { return activeOrders.size(); }
    long long getTotalSharesTraded() const { return totalSharesTraded; }
    long long getTotalTradedValue() const { return totalTradedValue; }
};
