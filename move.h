#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <iostream>

using namespace std; 

struct Move {
    string name;
    string type;
    int minDamage;
    int maxDamage;
    int currentPP;
    int maxPP;

    Move(string n = "Struggle", string t = "Normal", int minD = 5, int maxD = 10, int pp = 1)
        : name(n), type(t), minDamage(minD), maxDamage(maxD), currentPP(pp), maxPP(pp) {}

    void displayMove() const;
};

#endif // MOVE_H
