// Display.h - all printing of the order book, trades and market summary
#pragma once

#include <iostream>
#include <iomanip>
#include <string>

#include "OrderBook.h"

inline void displayOrderBook(const OrderBook& book) {
    cout << "\n================ ORDER BOOK ================\n";
    cout << "SELL ORDERS (best / lowest ask first)\n";

    if (book.getSellLevels().empty()) {
        cout << "  No active sell orders.\n";
    } else {
        cout << left << setw(12) << "Price"
             << setw(14) << "Order ID"
             << setw(16) << "Remaining Qty"
             << "Sequence\n";
        cout << string(52, '-') << "\n";

        for (const auto& level : book.getSellLevels()) {
            for (const Order& order : level.second) {
                cout << left << setw(12) << order.price
                     << setw(14) << order.id
                     << setw(16) << order.remainingQuantity
                     << order.sequence << "\n";
            }
        }
    }

    cout << "---------------------------------------------\n";
    cout << "BUY ORDERS (best / highest bid first)\n";

    if (book.getBuyLevels().empty()) {
        cout << "  No active buy orders.\n";
    } else {
        cout << left << setw(12) << "Price"
             << setw(14) << "Order ID"
             << setw(16) << "Remaining Qty"
             << "Sequence\n";
        cout << string(52, '-') << "\n";

        for (const auto& level : book.getBuyLevels()) {
            for (const Order& order : level.second) {
                cout << left << setw(12) << order.price
                     << setw(14) << order.id
                     << setw(16) << order.remainingQuantity
                     << order.sequence << "\n";
            }
        }
    }

    cout << "=============================================\n";
}

inline void displayTradeHistory(const OrderBook& book) {
    const vector<Trade>& tradeHistory = book.getTradeHistory();

    cout << "\n================ TRADE HISTORY ================\n";

    if (tradeHistory.empty()) {
        cout << "No trades have been executed yet.\n";
        return;
    }

    cout << left << setw(10) << "Trade ID"
         << setw(12) << "Buy ID"
         << setw(12) << "Sell ID"
         << setw(12) << "Price"
         << "Quantity\n";
    cout << string(58, '-') << "\n";

    for (const Trade& trade : tradeHistory) {
        cout << left << setw(10) << trade.tradeId
             << setw(12) << trade.buyOrderId
             << setw(12) << trade.sellOrderId
             << setw(12) << trade.price
             << trade.quantity << "\n";
    }

    cout << "Total trades: " << tradeHistory.size() << "\n";
    cout << "Total shares traded: " << book.getTotalSharesTraded() << "\n";
    cout << "================================================\n";
}

inline void displayMarketSummary(const OrderBook& book) {
    const auto& buyLevels = book.getBuyLevels();
    const auto& sellLevels = book.getSellLevels();
    const vector<Trade>& tradeHistory = book.getTradeHistory();
    long long totalSharesTraded = book.getTotalSharesTraded();
    long long totalTradedValue = book.getTotalTradedValue();

    cout << "\n================ MARKET SUMMARY ================\n";

    if (buyLevels.empty()) {
        cout << "Best bid: None\n";
    } else {
        cout << "Best bid: Rs. " << buyLevels.begin()->first
             << " | Quantity at best bid: "
             << book.quantityAtTopOfBook(Side::BUY) << "\n";
    }

    if (sellLevels.empty()) {
        cout << "Best ask: None\n";
    } else {
        cout << "Best ask: Rs. " << sellLevels.begin()->first
             << " | Quantity at best ask: "
             << book.quantityAtTopOfBook(Side::SELL) << "\n";
    }

    if (!buyLevels.empty() && !sellLevels.empty()) {
        int spread = sellLevels.begin()->first - buyLevels.begin()->first;
        cout << "Bid-ask spread: Rs. " << spread << "\n";
    } else {
        cout << "Bid-ask spread: Not available (one side of the book is empty).\n";
    }

    cout << "Active orders: " << book.getActiveOrderCount() << "\n";
    cout << "Trades executed: " << tradeHistory.size() << "\n";
    cout << "Total shares traded: " << totalSharesTraded << "\n";
    cout << "Total traded value: Rs. " << totalTradedValue << "\n";

    if (totalSharesTraded > 0) {
        double averagePrice = static_cast<double>(totalTradedValue)
                            / static_cast<double>(totalSharesTraded);
        cout << fixed << setprecision(2);
        cout << "Volume-weighted average execution price: Rs. "
             << averagePrice << "\n";
    } else {
        cout << "Volume-weighted average execution price: Not available yet.\n";
    }

    if (!tradeHistory.empty()) {
        const Trade& latestTrade = tradeHistory.back();
        cout << "Latest execution: " << latestTrade.quantity
             << " share(s) at Rs. " << latestTrade.price << "\n";
    }

    cout << "================================================\n";
}
