#include "pokemon.h"
#include <iostream> // For cout, endl


void Pokemon::displaySummary() const {
    cout << name << " (" << type1 << (type2.empty() ? "" : "/" + type2) << ") - HP: " << currentHP << "/" << maxHP << (isFainted ? " (Fainted)" : "");
}

void Pokemon::displayDetailed() const {
    cout << "----------------------------------------" << endl;
    cout << name << " (ID: " << id << ")" << endl;
    cout << "Type: " << type1 << (type2.empty() ? "" : "/" + type2) << endl;
    cout << "HP: " << currentHP << "/" << maxHP << (isFainted ? " (Fainted)" : "") << endl;
    cout << "\nASCII Art:\n" << asciiArt << endl;
    cout << "\nMoves:" << endl;
    for (size_t i = 0; i < moveset.size(); ++i) {
        cout << i + 1 << ". ";
        moveset[i].displayMove();
        cout << endl;
    }
    cout << "----------------------------------------" << endl;
}

void Pokemon::learnMove(const Move& move) {
    if (moveset.size() < 4) {
        moveset.push_back(move);
    } else {
        cout << name << " already knows 4 moves. Cannot learn " << move.name << "." << endl;
        // TODO: Implement logic to replace a move if desired
    }
}

void Pokemon::heal() {
    currentHP = maxHP;
    isFainted = false;
    for (Move& move_ref : moveset) { 
        move_ref.currentPP = move_ref.maxPP;
    }
    cout << name << " has been fully healed!" << endl;
}
