#ifndef UTILS_H
#define UTILS_H

#include <string> 
#include <vector> 
#include <iostream> 
#include <limits>  
#include <random>  // For random - std:: removed
using namespace std;

// For clearing the console (basic, platform-dependent)
void clearScreen();

// Get a random integer within a range [min, max]
int getRandomInt(int min, int max);

// Get integer input from the user with basic validation
int getIntInput(const string& prompt, int minRange, int maxRange);

#endif // UTILS_H
