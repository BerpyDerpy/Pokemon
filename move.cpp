#include "move.h"
#include <iostream>
using namespace std;

void Move::displayMove() const {
    cout << name << " (" << type << ") - DMG: " << minDamage << "-" << maxDamage << " PP: " << currentPP << "/" << maxPP;
}