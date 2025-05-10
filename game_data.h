#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "pokemon.h"
#include <vector>
#include <map>
using namespace std;

// Global storage for all available Pokemon "templates"
extern vector<Pokemon> POKEMON_DATABASE;
// Global type chart
extern map<string, map<string, float>> TYPE_CHART;

// Function to initialize the Pokemon database and type chart
void initializeGameData();

// Helper to get a fresh copy of a Pokemon from the database
Pokemon getPokemonFromDB(int id);
Pokemon getPokemonFromDB(const string& name);


#endif // GAME_DATA_H