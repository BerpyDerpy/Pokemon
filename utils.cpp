#include "utils.h"
#include <cstdlib> // For system() - basic clear screen

using namespace std; 

// Static random number generator setup for better distribution
random_device rd_utils;
mt19937 gen_utils(rd_utils());

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    // Assume POSIX-compliant system (Linux, macOS)
    system("clear");
#endif
}

int getRandomInt(int min, int max) {
    if (min > max) {
        swap(min, max); 
    }
    uniform_int_distribution<> distrib(min, max);
    return distrib(gen_utils);
}

int getIntInput(string& prompt, int minRange, int maxRange) {
    int choice;
    while (true) {
        cout << prompt; 
        cin >> choice;  
        if (cin.good() && choice >= minRange && choice <= maxRange) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return choice;
        } else {
            cout << "Invalid input. Please enter a number between " << minRange << " and " << maxRange << "." << endl; 
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}
