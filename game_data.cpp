#include "game_data.h"
#include <iostream> 

//  global variables
vector<Pokemon> POKEMON_DATABASE;
map<string, map<string, float>> TYPE_CHART;

void initializeGameData() {
    // --- Initialize Type Chart (Minimal for the 4 example Pokemon) ---
    // Normal, Fire, Water, Grass, Poison, Electric
    // Rows are Attacking Type, Columns are Defending Type, Value is Multiplier

    // Normal
    TYPE_CHART["Normal"]["Rock"] = 0.5f; // Example, not used by current 4
    TYPE_CHART["Normal"]["Ghost"] = 0.0f; // Example

    // Fire
    TYPE_CHART["Fire"]["Fire"] = 0.5f;
    TYPE_CHART["Fire"]["Water"] = 0.5f;
    TYPE_CHART["Fire"]["Grass"] = 2.0f;

    // Water
    TYPE_CHART["Water"]["Fire"] = 2.0f;
    TYPE_CHART["Water"]["Water"] = 0.5f;
    TYPE_CHART["Water"]["Grass"] = 0.5f;

    // Grass
    TYPE_CHART["Grass"]["Fire"] = 0.5f;
    TYPE_CHART["Grass"]["Water"] = 2.0f;
    TYPE_CHART["Grass"]["Grass"] = 0.5f;
    TYPE_CHART["Grass"]["Poison"] = 0.5f; // Bulbasaur is Poison type

    // Electric
    TYPE_CHART["Electric"]["Water"] = 2.0f;
    TYPE_CHART["Electric"]["Grass"] = 0.5f;
    TYPE_CHART["Electric"]["Electric"] = 0.5f;
    TYPE_CHART["Electric"]["Ground"] = 0.0f; // Example

    // Poison (for Bulbasaur's attacks, or attacks against it)
    TYPE_CHART["Poison"]["Grass"] = 2.0f;
    TYPE_CHART["Poison"]["Poison"] = 0.5f;
    // Add more interactions for Poison if its moves are used or if other Pokemon are Poison type.


    // --- Initialize Pokemon Database (The 4 examples) ---
    Pokemon p;

    // 1. Bulbasaur
    p = Pokemon(1, "Bulbasaur", "Grass", "Poison", 45,
                "    /\\[_> ]/\n"
                "   | \\  / |\n"
                "   (_\\VV/_)\n"
                "     ||||  \n"
                "    (_)(_) \n"
                " (Bulbasaur)");
    p.learnMove(Move("Tackle", "Normal", 8, 12, 35));
    p.learnMove(Move("Vine Whip", "Grass", 10, 15, 25));
    p.learnMove(Move("Poison Powder", "Poison", 0, 0, 35)); // Status, for now no damage
    p.learnMove(Move("Growl", "Normal", 0, 0, 40));
    POKEMON_DATABASE.push_back(p);

    // 4. Charmander
    p = Pokemon(4, "Charmander", "Fire", "", 39, // HP slightly adjusted from a common source
                "   _.-_    \n"
                "  / /' )   \n"
                " ( ( (_) ) \n"
                "  \\ \\_)/ / \n"
                "   `---` (f)\n"
                "(Charmander)");
    p.learnMove(Move("Scratch", "Normal", 9, 13, 35));
    p.learnMove(Move("Ember", "Fire", 10, 16, 25));
    p.learnMove(Move("Growl", "Normal", 0, 0, 40));
    p.learnMove(Move("Smokescreen", "Normal", 0, 0, 20));
    POKEMON_DATABASE.push_back(p);

    // 7. Squirtle
    p = Pokemon(7, "Squirtle", "Water", "", 44,
                "    .--''--. \n"
                "   /________\\ \n"
                "  |  O    O  | \n"
                "  (   ____   ) \n"
                "   \\  `--'  /  \n"
                "    `.____.'   \n"
                " (Squirtle)");
    p.learnMove(Move("Tackle", "Normal", 8, 12, 35));
    p.learnMove(Move("Water Gun", "Water", 10, 16, 25));
    p.learnMove(Move("Tail Whip", "Normal", 0, 0, 30));
    p.learnMove(Move("Bubble", "Water", 7, 13, 30));
    POKEMON_DATABASE.push_back(p);

    // 25. Pikachu
    p = Pokemon(25, "Pikachu", "Electric", "", 35,
                "  Pika! (\\__/)\n"
                "        (o^.^)\n"
                "      z(_(\")(\")\n"
                "(Pikachu)");
    p.learnMove(Move("Thunder Shock", "Electric", 10, 15, 30));
    p.learnMove(Move("Quick Attack", "Normal", 9, 12, 30));
    p.learnMove(Move("Growl", "Normal", 0, 0, 40));
    p.learnMove(Move("Tail Whip", "Normal", 0, 0, 30));
    POKEMON_DATABASE.push_back(p);

    cout << "Game data initialized. " << POKEMON_DATABASE.size() << " Pokemon loaded." << endl;
}

Pokemon getPokemonFromDB(int id) {
    for (const auto& pkm_template : POKEMON_DATABASE) { // Changed var name
        if (pkm_template.id == id) {
            return pkm_template; // Return a copy
        }
    }
    cerr << "Error: Pokemon with ID " << id << " not found in database. Returning default." << endl;
    return Pokemon(); // Return a default Pokemon
}
Pokemon getPokemonFromDB(string& name) {
    for (const auto& pkm_template : POKEMON_DATABASE) { // Changed var name
        if (pkm_template.name == name) {
            return pkm_template; // Return a copy
        }
    }
    cerr << "Error: Pokemon with name " << name << " not found in database. Returning default." << endl;
    return Pokemon(); // Return a default Pokemon
}
