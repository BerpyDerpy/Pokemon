#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "pokemon.h" // Player has Pokemon
using namespace std;

struct Player {
    string name;
    vector<Pokemon> pokemonParty;      // All Pokemon the player owns (up to 6 for a standard battle)
    vector<int> activePokemonIndices; // Indices of Pokemon chosen for the current battle/tournament round
                                           // relative to pokemonParty. This makes switching easier.
    Pokemon* currentPokemon;               // Pointer to the currently active Pokemon in battle from pokemonParty
    int money;
    // vector<string> items; // For future item implementation

    Player(string n = "Trainer") : name(n), currentPokemon(nullptr), money(1000) {}

    void displayPartySummary() const;
    bool canBattle() const; // Checks if any Pokemon in the active party can battle
    void healParty();       // Heals all Pokemon in pokemonParty
    void chooseActivePokemonForBattle(int teamSize); // Allows player to select 'teamSize' Pokemon for activePokemonIndices
    Pokemon* getFirstAvailablePokemon(); // Gets the next available Pokemon from active list
};

#endif // PLAYER_H