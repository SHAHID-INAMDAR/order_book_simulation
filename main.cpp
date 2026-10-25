// main.cpp - runs the program: shows the menu and calls the other files
//
// Compile:  g++ -std=c++17 main.cpp -o orderflow
// Run:      ./orderflow

#include <iostream>

#include "Types.h"
#include "OrderBook.h"
#include "Display.h"
#include "Menu.h"

int main() {
    OrderBook orderBook;

    showBanner();

    while (true) {
        showMenu();
        int choice = readInteger("Enter your choice: ");

        if (choice == 1 || choice == 2) {
            int price = readInteger("Enter limit price (whole rupees): ");
            int quantity = readInteger("Enter quantity (shares): ");

            if (choice == 1) {
                orderBook.submitOrder(Side::BUY, price, quantity);
            } else {
                orderBook.submitOrder(Side::SELL, price, quantity);
            }
        } else if (choice == 3) {
            int orderId = readInteger("Enter the active order ID to cancel: ");
            orderBook.cancelOrder(orderId);
        } else if (choice == 4) {
            displayOrderBook(orderBook);
        } else if (choice == 5) {
            displayTradeHistory(orderBook);
        } else if (choice == 6) {
            displayMarketSummary(orderBook);
        } else if (choice == 7) {
            loadDemoScenario(orderBook);
        } else if (choice == 0) {
            cout << "\nThank you for using OrderFlow. Goodbye!\n";
            break;
        } else {
            cout << "Invalid menu option. Please choose a listed option.\n";
        }
    }

    return 0;
}
