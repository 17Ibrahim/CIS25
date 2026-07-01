#include <iostream> 

using namespace std; 

int main() { 

    float n1, n2, n3; // added commas

    cout << "Enter three numbers: "; 

    cin >> n1 >> n2 >> n3; // fixed the >>

    // nested if statement
    if (n1 >= n2) { // fixed == sign issues

        if (n1 >= n3) { 
            cout << "Largest number: " << n1; 
        }
        else { 
            cout << "Largest number: " << n3; 
        }

    }
    else { 

        if (n2 >= n3) { 
            cout << "Largest number: " << n2; //missing ;
        }
        else { 
            cout << "Largest number: " << n3; 
        }
    }

    return 0; 
}