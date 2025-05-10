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

// --- Player Setup ---
Player setupPlayer(const string& playerNumberName, int numPokemonToChoose) {
    string name;
    cout << "Enter name for " << playerNumberName << ": ";
    getline(cin, name);
    if (name.empty()) name = playerNumberName; // Default name if empty

    Player newPlayer(name);
    cout << "\n" << newPlayer.name << ", choose your " << numPokemonToChoose << " Pokemon:" << endl;

    // Display available Pokemon
    cout << "Available Pokemon:" << endl;
    for (size_t i = 0; i < POKEMON_DATABASE.size(); ++i) {
        cout << i + 1 << ". " << POKEMON_DATABASE[i].name << " (" << POKEMON_DATABASE[i].type1
                  << (POKEMON_DATABASE[i].type2.empty() ? "" : "/" + POKEMON_DATABASE[i].type2) << ")" << endl;
    }

    vector<int> chosen_ids; // To prevent picking the same DB pokemon multiple times if desired
    for (int i = 0; i < numPokemonToChoose; ++i) {
        int choice = -1;
        bool already_chosen = true;
        while(already_chosen){
            choice = getIntInput("Choose Pokemon #" + to_string(i + 1) + " (1-" + to_string(POKEMON_DATABASE.size()) + "): ", 1, POKEMON_DATABASE.size());
            already_chosen = false;
            for(int id : chosen_ids){
                if(id == choice -1) {
                    cout << "You've already selected that Pokemon. Pick another.\n";
                    already_chosen = true;
                    break;
                }
            }
        }
        chosen_ids.push_back(choice-1);
        newPlayer.pokemonParty.push_back(getPokemonFromDB(POKEMON_DATABASE[choice - 1].id)); // Add a copy
        cout << POKEMON_DATABASE[choice - 1].name << " added to your party!" << endl;
    }
    // For battles, activePokemonIndices will be set up right before the battle/round
    // Or, for simple 1v1, we can set it up here too or just use the first few.
    newPlayer.chooseActivePokemonForBattle(min(numPokemonToChoose, (int)newPlayer.pokemonParty.size()));
    return newPlayer;
}

// --- Gym Leader Setup ---
Player setupGymLeader(const string& name, const vector<int>& pokemonIds, int hpBonus = 0) {
    Player gymLeader(name);
    for (int id : pokemonIds) {
        Pokemon p = getPokemonFromDB(id);
        p.maxHP += hpBonus; // Gym leaders might have stronger Pokemon
        p.currentHP = p.maxHP;
        gymLeader.pokemonParty.push_back(p);
    }
    gymLeader.chooseActivePokemonForBattle(gymLeader.pokemonParty.size()); // Gym leader uses all their Pokemon
    return gymLeader;
}


int main() {
    initializeGameData(); // IMPORTANT: Load Pokemon, types, etc.

    // Check if database loaded
    if (POKEMON_DATABASE.empty()) {
        cout << "Failed to load Pokemon database. Exiting." << endl;
        return 1;
    }

    vector<Player> gamePlayers; // Store players created for tournament registration

    while (true) {
        clearScreen();
        cout << "====== Pokemon C++ Battle ======" << endl;
        cout << "1. 1v1 Battle (Co-op)" << endl;
        cout << "2. Gym Battle" << endl;
        cout << "3. Start Tournament" << endl;
        cout << "4. View All Pokemon Info" << endl;
        cout << "5. Register Player for Tournament" << endl;
        cout << "6. View Registered Tournament Players" << endl;
        cout << "7. Exit" << endl;

        int choice = getIntInput("Enter your choice: ", 1, 7);

        if (choice == 1) { // 1v1 Battle
            clearScreen();
            cout << "--- Player 1 Setup ---" << endl;
            Player player1 = setupPlayer("Player 1", 3); // 3 Pokemon for a quick 1v1
            cout << "\n--- Player 2 Setup ---" << endl;
            Player player2 = setupPlayer("Player 2", 3);
            player1.currentPokemon = player1.getFirstAvailablePokemon(); // Ensure current Pokemon is set
            player2.currentPokemon = player2.getFirstAvailablePokemon();
            startBattle(player1, player2);
            getIntInput("\nBattle finished. Press Enter to return to menu...",0,0);
        }
        else if (choice == 2) { // Gym Battle
            clearScreen();
            cout << "--- Player Setup ---" << endl;
            Player challenger = setupPlayer("Challenger", 6); // Full team for gym
            challenger.currentPokemon = challenger.getFirstAvailablePokemon();

            // Example Gym Leader (Brock-like)
            Player gymLeader = setupGymLeader("Gym Leader Brock", {74, 95}, 20); // Geodude, Onix with HP bonus
            // IDs 74 (Geodude) and 95 (Onix) would need to be in your POKEMON_DATABASE
            // For now, let's use what we have:
            if (POKEMON_DATABASE.size() >=2) { // Ensure we have at least 2 pokemon for the gym leader
                 gymLeader = setupGymLeader("Gym Leader Stone", {POKEMON_DATABASE[0].id, POKEMON_DATABASE[1].id}, 10);
            } else {
                cout << "Not enough Pokemon in DB for Gym Leader. Using defaults." << endl;
                 gymLeader = setupGymLeader("Gym Leader Default", {POKEMON_DATABASE[0].id}, 10);
            }
            gymLeader.currentPokemon = gymLeader.getFirstAvailablePokemon();


            cout << "\n--- Gym Battle: " << challenger.name << " vs " << gymLeader.name << " ---" << endl;
            startBattle(challenger, gymLeader, true); // true indicates it's a gym battle (for AI behavior)
            getIntInput("\nGym Battle finished. Press Enter to return to menu...",0,0);

        }
        else if (choice == 3) { // Start Tournament
            if (gamePlayers.size() < 2) {
                cout << "Not enough players registered for a tournament. Please register at least 2 players." << endl;
            } else {
                startTournament(gamePlayers); // Pass the vector of registered players
            }
            getIntInput("\nTournament finished or exited. Press Enter to return to menu...",0,0);
        }
        else if (choice == 4) { // View All Pokemon Info
            clearScreen();
            if (POKEMON_DATABASE.empty()) {
                cout << "Pokemon database is empty!" << endl;
            } else {
                for (const auto& pkm : POKEMON_DATABASE) {
                    pkm.displayDetailed();
                    getIntInput("Press Enter to see next Pokemon (or q then Enter to quit view)...",0,0); // Basic pagination
                     // Check if user wants to quit viewing:
                    if (cin.peek() == 'q') {
                        string quit_signal;
                        cin >> quit_signal; // consume 'q'
                        break;
                    }
                }
            }
            getIntInput("\nFinished viewing. Press Enter to return to menu...",0,0);
        }
        else if (choice == 5) { // Register Player
            clearScreen();
            if (gamePlayers.size() >= 8) { // Arbitrary limit for example
                cout << "Maximum tournament participants reached (8)." << endl;
            } else {
                cout << "--- Register Player for Tournament ---" << endl;
                gamePlayers.push_back(setupPlayer("Tournament Player " + to_string(gamePlayers.size() + 1), 6)); // Players select 6 for their main party
                cout << gamePlayers.back().name << " registered successfully with their team!" << endl;
            }
            getIntInput("\nPress Enter to return to menu...",0,0);

        } else if (choice == 6) { // View Registered
            clearScreen();
            cout << "--- Registered Tournament Players ---" << endl;
            if (gamePlayers.empty()) {
                cout << "No players registered yet." << endl;
            } else {
                for(const auto& p : gamePlayers) {
                    p.displayPartySummary();
                    cout << "---------------------\n";
                }
            }
            getIntInput("\nPress Enter to return to menu...",0,0);
        }
        else if (choice == 7) { // Exit
            cout << "Exiting Pokemon C++ Battle. Thanks for playing!" << endl;
            break;
        }
    }

    return 0;
}