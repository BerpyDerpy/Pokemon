#ifndef MOVE_H
#define MOVE_H

#include <string>

struct Move {
    std::string name;
    std::string type;
    int minDamage;
    int maxDamage;
    int currentPP;
    int maxPP;

    Move(std::string n = "Struggle", std::string t = "Normal", int minD = 5, int maxD = 10, int pp = 1)
        : name(n), type(t), minDamage(minD), maxDamage(maxD), currentPP(pp), maxPP(pp) {}

    void displayMove() const;
};

#endif // MOVE_H