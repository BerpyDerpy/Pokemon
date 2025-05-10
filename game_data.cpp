#include "game_data.h"
#include <iostream> // For potential error messages
using namespace std;

// Define the global variables
vector<Pokemon> POKEMON_DATABASE;
map<string, map<string, float>> TYPE_CHART;

void initializeGameData() {
    // --- Initialize Type Chart (Example - Expand this significantly!) ---
    // Rows are Attacking Type, Columns are Defending Type, Value is Multiplier
    TYPE_CHART["Normal"]["Rock"] = 0.5f;
    TYPE_CHART["Normal"]["Ghost"] = 0.0f; // Immune
    TYPE_CHART["Normal"]["Steel"] = 0.5f;

    TYPE_CHART["Fire"]["Fire"] = 0.5f;
    TYPE_CHART["Fire"]["Water"] = 0.5f;
    TYPE_CHART["Fire"]["Grass"] = 2.0f;
    TYPE_CHART["Fire"]["Ice"] = 2.0f;
    TYPE_CHART["Fire"]["Bug"] = 2.0f;
    TYPE_CHART["Fire"]["Rock"] = 0.5f;
    TYPE_CHART["Fire"]["Dragon"] = 0.5f;
    TYPE_CHART["Fire"]["Steel"] = 2.0f;

    TYPE_CHART["Water"]["Fire"] = 2.0f;
    TYPE_CHART["Water"]["Water"] = 0.5f;
    TYPE_CHART["Water"]["Grass"] = 0.5f;
    TYPE_CHART["Water"]["Ground"] = 2.0f;
    TYPE_CHART["Water"]["Rock"] = 2.0f;
    TYPE_CHART["Water"]["Dragon"] = 0.5f;

    TYPE_CHART["Grass"]["Fire"] = 0.5f;
    TYPE_CHART["Grass"]["Water"] = 2.0f;
    TYPE_CHART["Grass"]["Grass"] = 0.5f;
    TYPE_CHART["Grass"]["Poison"] = 0.5f;
    TYPE_CHART["Grass"]["Ground"] = 2.0f;
    TYPE_CHART["Grass"]["Flying"] = 0.5f;
    TYPE_CHART["Grass"]["Bug"] = 0.5f;
    TYPE_CHART["Grass"]["Rock"] = 2.0f;
    TYPE_CHART["Grass"]["Dragon"] = 0.5f;
    TYPE_CHART["Grass"]["Steel"] = 0.5f;

    TYPE_CHART["Electric"]["Water"] = 2.0f;
    TYPE_CHART["Electric"]["Electric"] = 0.5f;
    TYPE_CHART["Electric"]["Grass"] = 0.5f;
    TYPE_CHART["Electric"]["Ground"] = 0.0f;
    TYPE_CHART["Electric"]["Flying"] = 2.0f;
    TYPE_CHART["Electric"]["Dragon"] = 0.5f;

    // ... and so on for all types: Ice, Fighting, Poison, Ground, Flying,
    // Psychic, Bug, Rock, Ghost, Dragon, Steel, Fairy.
    // Remember to handle cases where a type is not in the map (default to 1.0f effectiveness).

    // --- Initialize Pokemon Database (Example of a few - THIS IS THE BIGGEST DATA ENTRY PART) ---
    Pokemon p;

    // 1. Bulbasaur
    p = Pokemon(1, "Bulbasaur", "Grass", "Poison", 45,
                "    /\\[_> ]/\n"
                "   | \\  / |\n"
                "   (_\\VV/_)\n"
                "     ||||  \n"
                "    (_)(_) \n"
                " (Bulbasaur - very basic)");
    p.learnMove(Move("Tackle", "Normal", 8, 12, 35));
    p.learnMove(Move("Vine Whip", "Grass", 10, 15, 25));
    p.learnMove(Move("Growl", "Normal", 0, 0, 40)); // Status move, damage 0 for now
    p.learnMove(Move("Leech Seed", "Grass", 5, 5, 10)); // Special effect, for now just damage
    POKEMON_DATABASE.push_back(p);

    // 4. Charmander
    p = Pokemon(4, "Charmander", "Fire", "", 40,
                "   _.-_    \n"
                "  / /' )   \n"
                " ( ( (_) ) \n"
                "  \\ \\_)/ / \n"
                "   `---` tail-fire\n"
                "(Charmander - very basic)");
    p.learnMove(Move("Scratch", "Normal", 9, 13, 35));
    p.learnMove(Move("Ember", "Fire", 10, 16, 25));
    p.learnMove(Move("Growl", "Normal", 0, 0, 40));
    p.learnMove(Move("Smokescreen", "Normal", 0, 0, 20));
    POKEMON_DATABASE.push_back(p);

    // 7. Squirtle
    p = Pokemon(7, "Squirtle", "Water", "", 44,
                "    .--''--. \n"
                "   /________\\ \n"
                "  |☐        ☐| \n"
                "  (   O  O   ) \n"
                "   \\  `--'  /  \n"
                "    `.____.'   \n"
                " (Squirtle - very basic)");
    p.learnMove(Move("Tackle", "Normal", 8, 12, 35));
    p.learnMove(Move("Water Gun", "Water", 10, 16, 25));
    p.learnMove(Move("Tail Whip", "Normal", 0, 0, 30));
    p.learnMove(Move("Bubble", "Water", 7, 13, 30));
    POKEMON_DATABASE.push_back(p);

    // 25. Pikachu
    p = Pokemon(25, "Pikachu", "Electric", "", 35,
                "  (\\__/)\n"
                "  (o^.^)\n"
                " z(_(\")(\")\n"
                "(Pikachu)");
    p.learnMove(Move("Thunder Shock", "Electric", 10, 15, 30));
    p.learnMove(Move("Quick Attack", "Normal", 9, 12, 30)); // Add priority later if desired
    p.learnMove(Move("Growl", "Normal", 0, 0, 40));
    p.learnMove(Move("Tail Whip", "Normal", 0, 0, 30));
    POKEMON_DATABASE.push_back(p);

    // ... Add all 151 Pokemon with their types, HP, ASCII art, and learnable moves ...
    // This is a very time-consuming step.
    // For moves, define their type, min/max damage, and PP.

    cout << "Game data initialized. " << POKEMON_DATABASE.size() << " Pokemon loaded." << endl;
}

Pokemon getPokemonFromDB(int id) {
    for (const auto& pkm : POKEMON_DATABASE) {
        if (pkm.id == id) {
            return pkm; // Return a copy
        }
    }
    cerr << "Error: Pokemon with ID " << id << " not found in database. Returning default." << endl;
    return Pokemon(); // Return a default Pokemon
}
Pokemon getPokemonFromDB(const string& name) {
    for (const auto& pkm : POKEMON_DATABASE) {
        if (pkm.name == name) {
            return pkm; // Return a copy
        }
    }
    cerr << "Error: Pokemon with name " << name << " not found in database. Returning default." << endl;
    return Pokemon(); // Return a default Pokemon
}