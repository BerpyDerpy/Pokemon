#include <iostream>
#include <vector>
#include <string>
#include "utils.h"
#include "pokemon.h"
#include "player.h"
#include "game_data.h"    // For initializeGameData and POKEMON_DATABASE
#include "battle_logic.h" // For startBattle
#include "tournament.h"   // For startTournament
using namespace std;

// Player Setup 
Player setupPlayer( string& playerNumberName, int numPokemonToChooseFromDB) {
    string name_input; 
    cout << "Enter name for " << playerNumberName << ": ";
   
    getline(cin >> ws, name_input); 
   // If names with spaces are needed, original getline(cin, name_input) after a cin.ignore() is better.
    if (name_input.empty()) name_input = playerNumberName;

    Player newPlayer(name_input);
    cout << "\n" << newPlayer.name << ", choose your " << numPokemonToChooseFromDB << " Pokemon:" << endl;

    if (POKEMON_DATABASE.empty()) {
        cout << "Error: Pokemon Database is empty! Cannot choose Pokemon." << endl;
        return newPlayer; // Return player with no Pokemon
    }

    cout << "Available Pokemon:" << endl;
    for (size_t i = 0; i < POKEMON_DATABASE.size(); ++i) {
        cout << i + 1 << ". " << POKEMON_DATABASE[i].name << " (" << POKEMON_DATABASE[i].type1
                  << (POKEMON_DATABASE[i].type2.empty() ? "" : "/" + POKEMON_DATABASE[i].type2) << ")" << endl;
    }

    vector<int> chosen_db_indices; // To prevent picking the exact same DB entry if unique copies are desired
    for (int i = 0; i < numPokemonToChooseFromDB; ++i) {
        if (newPlayer.pokemonParty.size() >= 4) { // Max 4 Pokemon in party
            cout << "Party is full (6 Pokemon)." << endl;
            break;
        }
        int choice_num = -1; // User's 1-based choice
        int db_idx = -1; // 0-based index for POKEMON_DATABASE
        bool already_chosen_this_session = true;

        while(already_chosen_this_session){
            choice_num = getIntInput("Choose Pokemon #" + to_string(i + 1) + " (1-" + to_string(POKEMON_DATABASE.size()) + "): ", 1, POKEMON_DATABASE.size());
            db_idx = choice_num -1;
            already_chosen_this_session = false; // Assume not chosen
            for(int prev_idx : chosen_db_indices){ // Check if this DB Pokemon (by its original index) was already taken *in this setup session*
                if(prev_idx == db_idx) {
                    cout << "You've already selected that specific Pokemon from the list for your team. Pick another.\n";
                    already_chosen_this_session = true;
                    break;
                }
            }
        }
        chosen_db_indices.push_back(db_idx);
        newPlayer.pokemonParty.push_back(getPokemonFromDB(POKEMON_DATABASE[db_idx].id)); // Add a copy from DB
        cout << POKEMON_DATABASE[db_idx].name << " added to your party!" << endl;
    }
    // For 1v1 or Gyms, they choose their active team right before battle or use all.
    // For tournaments, chooseActivePokemonForBattle is called per round.
    // Let's ensure the player has *some* active pokemon selected if they have a party.
    if (!newPlayer.pokemonParty.empty()) {
         newPlayer.chooseActivePokemonForBattle(min((int)newPlayer.pokemonParty.size(), 3)); // Default to 3 active for non-tournament, or all if less than 3.
    }

    return newPlayer;
}

Player setupGymLeader(const string& name, const vector<int>& pokemonDBIds, int hpBonus = 0) {
    Player gymLeader(name);
    for (int id : pokemonDBIds) {
        Pokemon p = getPokemonFromDB(id); // Fetches by unique ID
        if (p.name == "MissingNo") { // Check if default Pokemon was returned
            cout << "Warning: Could not find Pokemon with ID " << id << " for Gym Leader. Skipping." << endl;
            continue;
        }
        p.maxHP += hpBonus;
        p.currentHP = p.maxHP;
        gymLeader.pokemonParty.push_back(p);
    }
    if (!gymLeader.pokemonParty.empty()) {
        gymLeader.chooseActivePokemonForBattle(gymLeader.pokemonParty.size()); // Gym leader uses all their chosen Pokemon
        gymLeader.currentPokemon = gymLeader.getFirstAvailablePokemon();
    }
    return gymLeader;
}


int main() {
    initializeGameData();

    if (POKEMON_DATABASE.empty()) {
        cout << "Critical Error: Pokemon database is empty after initialization. Exiting." << endl;
        return 1;
    }

    vector<Player> gamePlayersTournamentRegistry; // For tournament

    while (true) {
        clearScreen(); 
        cout << "\n====== Pokemon C++ Battle (Bare Minimum) ======" << endl;
        cout << "1. 1v1 Battle (Co-op)" << endl;
        cout << "2. Gym Battle (vs Brock-like)" << endl;
        cout << "3. Start Tournament (Basic)" << endl;
        cout << "4. View All Pokemon Info (" << POKEMON_DATABASE.size() << " available)" << endl;
        cout << "5. Register Player for Tournament" << endl;
        cout << "6. Exit" << endl;

        int choice = getIntInput("Enter your choice: ", 1, 6);

        if (choice == 1) {
            clearScreen();
            cout << "--- Player 1 Setup ---" << endl;
            Player player1 = setupPlayer("Player 1", 3); // Choose 3 for a 1v1
            cout << "\n--- Player 2 Setup ---" << endl;
            Player player2 = setupPlayer("Player 2", 3);

            if (player1.pokemonParty.empty() || player2.pokemonParty.empty()) {
                cout << "One or both players have no Pokemon. Cannot battle." << endl;
            } else {
                // Ensure currentPokemon is set from the active list created in setupPlayer or chooseActivePokemonForBattle
                player1.currentPokemon = player1.getFirstAvailablePokemon();
                player2.currentPokemon = player2.getFirstAvailablePokemon();
                if(player1.currentPokemon && player2.currentPokemon) {
                    startBattle(player1, player2);
                } else {
                    cout << "Could not set starting Pokemon for battle." << endl;
                }
            }
            getIntInput("\nBattle finished. Press Enter to return to menu...",0,0);
        }
        else if (choice == 2) { // Gym Battle
            clearScreen();
            cout << "--- Challenger Setup ---" << endl;
            Player challenger = setupPlayer("Challenger", 6); // Full team of 6 for gym

            if (challenger.pokemonParty.empty()) {
                cout << "Challenger has no Pokemon. Cannot challenge Gym." << endl;
            } else {
                // Setup Gym Leader with available Pokemon (IDs 1 and 4: Bulbasaur, Charmander)
                Player gymLeader;
                if (POKEMON_DATABASE.size() >= 2) { // Ensure at least two types of pokemon exist in DB for variety
                     gymLeader = setupGymLeader("Gym Leader Brock", {POKEMON_DATABASE[0].id, POKEMON_DATABASE[1].id}, 10);
                } else if (!POKEMON_DATABASE.empty()) {
                     gymLeader = setupGymLeader("Gym Leader Rock", {POKEMON_DATABASE[0].id}, 10); // Only one type
                } else {
                    cout << "No Pokemon in DB to create Gym Leader!" << endl;
                    continue;
                }


                if (gymLeader.pokemonParty.empty()) {
                    cout << "Gym Leader has no Pokemon. Cannot battle." << endl;
                } else {
                    challenger.currentPokemon = challenger.getFirstAvailablePokemon(); // Make sure it's set
                    gymLeader.currentPokemon = gymLeader.getFirstAvailablePokemon();   // Make sure it's set
                     if(challenger.currentPokemon && gymLeader.currentPokemon) {
                        startBattle(challenger, gymLeader, true);
                    } else {
                        cout << "Could not set starting Pokemon for Gym battle." << endl;
                    }
                }
            }
            getIntInput("\nGym Battle finished. Press Enter to return to menu...",0,0);
        }
        else if (choice == 3) { // Start Tournament
             if (gamePlayersTournamentRegistry.size() < 2) {
                cout << "Not enough players registered for a tournament. Please register at least 2 players using option 5." << endl;
            } else {
                startTournament(gamePlayersTournamentRegistry);
            }
            getIntInput("\nPress Enter to return to menu...",0,0);
        }
        else if (choice == 4) { // View All Pokemon Info
            // clearScreen();
            if (POKEMON_DATABASE.empty()) {
                cout << "Pokemon database is empty!" << endl;
            } else {
                for (const auto& pkm_const_ref : POKEMON_DATABASE) { // Changed var name
                    pkm_const_ref.displayDetailed();
                    cout << "----------------------------------------" << endl;
                }
            }
            getIntInput("\nFinished viewing. Press Enter to return to menu...",0,0);
        }
        else if (choice == 5) { // Register Player for Tournament
            // clearScreen();
            if (gamePlayersTournamentRegistry.size() >= 8) { // Max 8 players for simplicity
                cout << "Tournament registry is full (8 players)." << endl;
            } else {
                cout << "--- Register Player for Tournament ---" << endl;
                // Players register with a full party of 6, then choose smaller teams for rounds.
                Player newTourneyPlayer = setupPlayer("Tournament Player " + to_string(gamePlayersTournamentRegistry.size() + 1), 6);
                if (!newTourneyPlayer.pokemonParty.empty()) {
                    gamePlayersTournamentRegistry.push_back(newTourneyPlayer);
                    cout << newTourneyPlayer.name << " registered successfully with their team!" << endl;
                } else {
                    cout << "Player setup failed or no Pokemon chosen. Not registered." << endl;
                }
            }
             getIntInput("\nPress Enter to return to menu...",0,0);
        }
        else if (choice == 6) { // Exit
            cout << "Exiting Pokemon C++ Battle. Thanks for playing!" << endl;
            break;
        }
    }

    return 0;
}
