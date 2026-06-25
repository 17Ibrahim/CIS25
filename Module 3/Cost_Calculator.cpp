#include <iostream>
using namespace std;

int main() 
{
    string itemName;
    int quantity;
    float cost;

    cout << "Enter Item Name: ";
    cin>> itemName;
    cout << "Enter Quantity: ";
    cin>> quantity;
    cout<< "Enter Cost $";
    cin>> cost;
    
    cout<< itemName << " TOTAL COST $" << quantity * cost;

    
    return 0;
}

