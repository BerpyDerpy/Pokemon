#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>
#include "move.h" // Pokemon has moves
using namespace std;

struct Pokemon {
    string name;
    string type1;
    string type2; // Can be empty if single type
    int currentHP;
    int maxHP;
    vector<Move> moveset; // Typically 4 moves
    string asciiArt;    // Multi-line string for ASCII art
    bool isFainted;
    int id; // Unique ID for easy lookup in a database

    Pokemon(int id = 0, string n = "MissingNo", string t1 = "Normal", string t2 = "", int hp = 50, string art = ":(")
        : id(id), name(n), type1(t1), type2(t2), maxHP(hp), currentHP(hp), asciiArt(art), isFainted(false) {}

    void displaySummary() const;
    void displayDetailed() const; // Shows ASCII art and moves
    void learnMove(const Move& move);
    void heal(); // Fully heals the Pokemon and restores PP
};

#endif // POKEMON_H