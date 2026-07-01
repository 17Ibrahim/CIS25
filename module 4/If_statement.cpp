#include <iostream>
using namespace std;

int main()
{
    int number;
    cout << "please enter whole number: ";
    cin >> number;
    if (number % 2 == 0)
    {
        cout << "you have entered a even number." << endl;
    }
    else
    {
        cout << "you have entered a odd number." << endl;
    }

    cout << "thanks. Bye.";

    system("pause>0");
}

