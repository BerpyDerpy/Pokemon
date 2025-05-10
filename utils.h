#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <limits> // Required for std::numeric_limits

// For clearing the console (basic, platform-dependent)
void clearScreen();

// Get a random integer within a range [min, max]
int getRandomInt(int min, int max);

// Get integer input from the user with basic validation
int getIntInput(const std::string& prompt, int minRange, int maxRange);

#endif // UTILS_H