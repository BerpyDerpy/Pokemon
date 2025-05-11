#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "pokemon.h" // Player has Pokemon

using namespace std; 
struct Player {
    string name;
    vector<Pokemon> pokemonParty;      // All Pokemon the player owns
    vector<int> activePokemonIndices; // Indices of Pokemon chosen for the current battle
    Pokemon* currentPokemon;          // Pointer to the currently active Pokemon in battle
    int money;

    Player(string n = "Trainer") : name(n), currentPokemon(nullptr), money(1000) {}

    void displayPartySummary() const;
    bool canBattle() const; // Checks if any Pokemon in the active party can battle
    void healParty();       // Heals all Pokemon in pokemonParty
    void chooseActivePokemonForBattle(int teamSize);
    Pokemon* getFirstAvailablePokemon();
};

#endif // PLAYER_H
