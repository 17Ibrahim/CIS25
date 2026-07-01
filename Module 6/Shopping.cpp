#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// =====================
// Item CLASS
// =====================
class Item {
public:
    string name;
    double price;
    int quantity;

    Item() {
        name = "";
        price = 0.0;
        quantity = 0;
    }

    Item(string n, double p, int q) {
        name = n;
        price = p;
        quantity = q;
    }

    double getTotalPrice() {
        return price * quantity;
    }
};

// =====================
// FUNCTION PROTOTYPES
// =====================
void showMenu();
void addItem();
void viewCheckout();

// =====================
// MAIN FUNCTION
// =====================
int main() {
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            addItem();
            break;
        case 2:
            viewCheckout();
            break;
        case 3:
            cout << "Thank you for shopping!\n";
            break;
        default:
            cout << "Invalid option. Try again.\n";
        }

        cout << endl;

    } while (choice != 3);

    return 0;
}

// =====================
// MENU
// =====================
void showMenu() {
    cout << "===== SHOPPING CART =====\n";
    cout << "1. Add Item to Cart\n";
    cout << "2. View Checkout\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

// =====================
// ADD ITEM (SAVE TO FILE)
// =====================
void addItem() {
    ofstream file("cart.txt", ios::app);

    if (!file) {
        cout << "Error opening file!\n";
        return;
    }

    string name;
    double price;
    int quantity;

    cout << "Enter item name: ";
    cin >> name;

    cout << "Enter item price: ";
    cin >> price;

    cout << "Enter item quantity: ";
    cin >> quantity;

    file << name << " " << price << " " << quantity << endl;

    file.close();

    cout << "Item added to cart!\n";
}

// =====================
// VIEW CHECKOUT (READ FILE)
// =====================
void viewCheckout() {
    ifstream file("cart.txt");

    if (!file) {
        cout << "No cart found.\n";
        return;
    }

    string name;
    double price;
    int quantity;

    double grandTotal = 0.0;

    cout << "\n===== CHECKOUT =====\n";

    while (file >> name >> price >> quantity) {
        Item item(name, price, quantity);

        double total = item.getTotalPrice();

        cout << "Item: " << name
            << " | Price: $" << price
            << " | Quantity: " << quantity
            << " | Total: $" << total << endl;

        grandTotal += total;
    }

    cout << "----------------------\n";
    cout << "Grand Total: $" << grandTotal << endl;

    file.close();
}