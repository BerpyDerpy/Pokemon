#include "tournament.h"
#include "battle_logic.h" // For startBattle
#include "utils.h"        // For getIntInput, clearScreen
#include <iostream>
#include <algorithm> // For std::shuffle, std::remove_if
using namespace std;

void displayBracket(const vector<Player*>& players, const string& roundName) {
    cout << "\n--- Tournament Bracket: " << roundName << " ---" << endl;
    if (players.empty()) {
        cout << "No players in this round." << endl;
        return;
    }
    for (size_t i = 0; i < players.size(); i += 2) {
        cout << players[i]->name;
        if (i + 1 < players.size()) {
            cout << " vs " << players[i+1]->name << endl;
        } else {
            cout << " (BYE)" << endl; // Player gets a bye
        }
    }
    cout << "------------------------------------" << endl;
}

vector<Player*> runTournamentRound(vector<Player*>& contestants, int teamSize, const string& roundName) {
    cout << "\n--- Starting " << roundName << " (Team Size: " << teamSize << ") ---" << endl;
    vector<Player*> winners;

    // Optional: Shuffle contestants for random pairings if not a seeded tournament
    // random_device rd;
    // mt19937 g(rd());
    // shuffle(contestants.begin(), contestants.end(), g);

    displayBracket(contestants, roundName);
    getIntInput("Press Enter to begin the round...",0,0);


    for (size_t i = 0; i < contestants.size(); i += 2) {
        clearScreen();
        if (i + 1 < contestants.size()) { // Standard match
            Player* p1 = contestants[i];
            Player* p2 = contestants[i+1];

            cout << "\nMatch: " << p1->name << " vs " << p2->name << endl;
            cout << p1->name << ", prepare your team of " << teamSize << " Pokemon!" << endl;
            p1->chooseActivePokemonForBattle(teamSize);
            cout << p2->name << ", prepare your team of " << teamSize << " Pokemon!" << endl;
            p2->chooseActivePokemonForBattle(teamSize);

            // Ensure first Pokemon are set correctly based on active choices
            p1->currentPokemon = p1->getFirstAvailablePokemon();
            p2->currentPokemon = p2->getFirstAvailablePokemon();


            Player* matchWinner = startBattle(*p1, *p2);

            if (matchWinner) {
                cout << "\n" << matchWinner->name << " wins the match and advances!" << endl;
                winners.push_back(matchWinner);

                // Pokemon Center for the winner
                cout << matchWinner->name << ", would you like to heal your Pokemon at the Pokemon Center? (y/n): ";
                char healChoice;
                cin >> healChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // consume newline
                if (healChoice == 'y' || healChoice == 'Y') {
                    matchWinner->healParty();
                }

                // Allow winner to change Pokemon for the next round (if there is one)
                // This logic is more for *between* rounds, but can be offered.
                // The actual choosing happens at the start of the *next* runTournamentRound call.
                 cout << matchWinner->name << ", you can adjust your team before the next round." << endl;


            } else {
                // Handle draws - for simplicity, maybe a coin flip or re-match, or one advances arbitrarily
                cout << "The match was a draw! For simplicity, " << p1->name << " advances (implement better draw resolution)." << endl;
                winners.push_back(p1); // Arbitrary winner on draw for now
                p1->healParty(); // Heal the 'winner'
            }
             // Option for loser to heal
            Player* loser = (matchWinner == p1) ? p2 : p1;
            if (matchWinner == nullptr) loser = p2; // If draw, p2 is arbitrarily loser for healing prompt
            cout << "\n" << loser->name << ", would you like to heal your Pokemon at the Pokemon Center? (y/n): ";
            char loserHealChoice;
            cin >> loserHealChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (loserHealChoice == 'y' || loserHealChoice == 'Y') {
                loser->healParty();
            }


        } else { // Player gets a bye
            Player* pBye = contestants[i];
            cout << "\n" << pBye->name << " gets a bye and advances to the next round!" << endl;
            winners.push_back(pBye);
            // Offer Pokemon Center
            cout << pBye->name << ", would you like to heal your Pokemon? (y/n): ";
            char healChoice;
            cin >> healChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (healChoice == 'y' || healChoice == 'Y') {
                pBye->healParty();
            }
        }
        if (contestants.size() > 1) getIntInput("Press Enter to continue to the next match/result...",0,0);
    }
    return winners;
}

void startTournament(vector<Player>& allRegisteredPlayers) {
    clearScreen();
    cout << "--- POKEMON TOURNAMENT ---" << endl;
    if (allRegisteredPlayers.size() < 2) {
        cout << "Not enough players for a tournament (minimum 2 required)." << endl;
        return;
    }

    vector<Player*> currentContestants;
    for(Player& p : allRegisteredPlayers) {
        currentContestants.push_back(&p); // Store pointers to original player objects
    }

    // --- Part 1: 3 Pokemon Battles ---
    vector<Player*> winnersPart1 = runTournamentRound(currentContestants, 3, "Round 1 (3v3)");

    if (winnersPart1.size() <= 1) {
        if (!winnersPart1.empty()) {
            cout << "\n🏆🏆🏆 The Grand Winner of the Tournament (Part 1) is " << winnersPart1[0]->name << "! 🏆🏆🏆" << endl;
            winnersPart1[0]->money += 1000; // Tournament prize
            cout << winnersPart1[0]->name << " received $1000!" << endl;
        } else {
            cout << "The first part of the tournament concluded without a clear winner." << endl;
        }
        getIntInput("Press Enter to return to the main menu...",0,0);
        return;
    }

    // --- Part 2: 6 Pokemon Battles ---
    cout << "\n\n--- Preparing for Part 2: 6v6 Battles! ---" << endl;
    getIntInput("Press Enter to continue...",0,0);

    vector<Player*> finalContestants = winnersPart1; // Winners from part 1 proceed
    vector<Player*> finalWinners = runTournamentRound(finalContestants, 6, "Final Round (6v6)");

    if (!finalWinners.empty() && finalWinners.size() == 1) {
        cout << "\n🏆🏆🏆 The ULTIMATE Grand Winner of the Tournament is " << finalWinners[0]->name << "! 🏆🏆🏆" << endl;
        finalWinners[0]->money += 2500; // Additional prize for winning part 2
        cout << finalWinners[0]->name << " received an additional $2500!" << endl;
    } else if (finalWinners.size() > 1) {
         cout << "\nThe tournament ended with multiple finalists after Part 2! (Further rounds needed or co-champions)" << endl;
    }
    else {
        cout << "The tournament concluded without a clear winner in the final round." << endl;
    }
    getIntInput("Press Enter to return to the main menu...",0,0);
}