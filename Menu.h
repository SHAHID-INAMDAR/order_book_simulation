// Menu.h - user input, menu text and the demo scenario
#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <cstdlib>

#include "OrderBook.h"

inline int readInteger(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            return value;
        }

        // Keep the program running if the user types letters instead of a number.
        if (cin.eof()) {
            cout << "\nInput ended. Exiting program.\n";
            exit(0);
        }

        cout << "Invalid input. Please enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

inline void showBanner() {
    cout << "==============================================\n";
    cout << " ORDERFLOW - TRADING ORDER BOOK SIMULATOR\n";
    cout << " C++ Data Structures and Algorithms Mini-Project\n";
    cout << "==============================================\n";
    cout << "Educational simulator only; no real market connection.\n";
    cout << "Prices use whole rupees and quantities use whole shares.\n";
}

inline void showMenu() {
    cout << "\n=============== ORDERFLOW MENU ===============\n";
    cout << "1. Submit BUY limit order\n";
    cout << "2. Submit SELL limit order\n";
    cout << "3. Cancel an active order\n";
    cout << "4. Display current order book\n";
    cout << "5. Display trade history\n";
    cout << "6. Display market summary\n";
    cout << "7. Load demo scenario\n";
    cout << "0. Exit\n";
    cout << "==============================================\n";
}

// A repeatable demo to help the team present the system.
// These orders go through the same matching functions as normal input.
inline void loadDemoScenario(OrderBook& book) {
    cout << "\nLoading demo orders into the current book...\n";
    cout << "Expected idea: buy 10 @ 100, then sell 6 @ 98, then sell 8 @ 100.\n";

    book.submitOrder(Side::BUY, 100, 10);
    book.submitOrder(Side::SELL, 98, 6);
    book.submitOrder(Side::SELL, 100, 8);

    cout << "\nDemo scenario submitted. Review the order book and trade history.\n";
}
