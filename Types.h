// Types.h - basic data types shared by the whole project
#pragma once

#include <string>

using namespace std;

enum class Side {
    BUY,
    SELL
};

inline string sideName(Side side) {
    if (side == Side::BUY) {
        return "BUY";
    }
    return "SELL";
}

struct Order {
    int id;
    Side side;
    int price;
    int remainingQuantity;
    long long sequence;
};

struct Trade {
    int tradeId;
    int buyOrderId;
    int sellOrderId;
    int price;
    int quantity;
};
