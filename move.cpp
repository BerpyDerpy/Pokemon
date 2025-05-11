#include "move.h"

void Move::displayMove() const {
    cout << name << " (" << type << ") - DMG: " << minDamage << "-" << maxDamage << " PP: " << currentPP << "/" << maxPP;
}
