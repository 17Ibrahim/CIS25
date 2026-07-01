#include <iostream>   
#include <cmath>      

using namespace std;

// Function prototypes
void showMenu();
void addNumbers();
void multiplyNumbers();
void showSquareRoot();

int main() {
    int option;

    do {
        showMenu();
        cin >> option;

        switch (option) {
        case 1:
            addNumbers();
            break;
        case 2:
            multiplyNumbers();
            break;
        case 3:
            showSquareRoot();
            break;
        case 4:
            cout << "Exiting program...\n";
            break;
        default:
            cout << "Invalid option. Try again.\n";
        }

        cout << endl;

    } while (option != 4);

    return 0;
}

// Shows menu
void showMenu() {
    cout << "===== MENU =====\n";
    cout << "1. Add two numbers\n";
    cout << "2. Multiply two numbers\n";
    cout << "3. Square root of a number\n";
    cout << "4. Exit\n";
    cout << "Enter option: ";
}

// Function 1
void addNumbers() {
    int a, b;
    cout << "Enter two numbers: ";
    cin >> a >> b;
    cout << "Sum = " << (a + b) << endl;
}

// Function 2
void multiplyNumbers() {
    int a, b;
    cout << "Enter two numbers: ";
    cin >> a >> b;
    cout << "Product = " << (a * b) << endl;
}

// Function 3
void showSquareRoot() {
    double num;
    cout << "Enter a number: ";
    cin >> num;

    if (num < 0) {
        cout << "Cannot take square root of a negative number.\n";
    }
    else {
        cout << "Square root = " << sqrt(num) << endl;
    }
}