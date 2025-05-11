#include "tournament.h"
#include "battle_logic.h" // For startBattle
#include "utils.h"        // For getIntInput, clearScreen
#include <iostream>       // For cout, endl
#include <algorithm>      // For shuffle (std::shuffle becomes shuffle)
#include <vector>         // For vector
#include <string>         // For string


void displayBracket( vector<Player*>& players,string& roundName) {
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
            cout << " (BYE)" << endl;
        }
    }
    cout << "------------------------------------" << endl;
}

vector<Player*> runTournamentRound(vector<Player*>& contestants, int teamSize, string& roundName) {
    cout << "\n--- Starting " << roundName << " (Team Size: " << teamSize << ") ---" << endl;
    vector<Player*> winners;

    // For bare minimum, shuffling can be optional.
    // random_device rd_tourney;
    // mt19937 g_tourney(rd_tourney());
    // shuffle(contestants.begin(), contestants.end(), g_tourney);

    displayBracket(contestants, roundName);
    getIntInput("Press Enter to begin the round...",0,0);

    for (size_t i = 0; i < contestants.size(); i += 2) {
        // clearScreen(); // Optional
        if (i + 1 < contestants.size()) {
            Player* p1 = contestants[i];
            Player* p2 = contestants[i+1];

            cout << "\nMatch: " << p1->name << " vs " << p2->name << endl;
            cout << p1->name << ", your active team for this " << teamSize << "v" << teamSize << " battle:" << endl;
            p1->chooseActivePokemonForBattle(teamSize); // Ensure active team is set/confirmed
            cout << p2->name << ", your active team for this " << teamSize << "v" << teamSize << " battle:" << endl;
            p2->chooseActivePokemonForBattle(teamSize);

            p1->currentPokemon = p1->getFirstAvailablePokemon(); // Crucial to set current from active
            p2->currentPokemon = p2->getFirstAvailablePokemon(); // Crucial to set current from active

            Player* matchWinner = startBattle(*p1, *p2);

            if (matchWinner) {
                cout << "\n" << matchWinner->name << " wins the match and advances!" << endl;
                winners.push_back(matchWinner);
                cout << matchWinner->name << ", would you like to heal your Pokemon? (y/n): ";
                char healChoice;
                cin >> healChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (healChoice == 'y' || healChoice == 'Y') {
                    matchWinner->healParty();
                }
            } else {
                cout << "The match was a draw! Arbitrarily, " << p1->name << " advances (improve draw logic)." << endl;
                winners.push_back(p1);
                p1->healParty();
            }
            Player* loser = (matchWinner == p1) ? p2 : p1;
            if(matchWinner == nullptr) loser = p2; // arbitrary loser for healing prompt on draw

            cout << "\n" << loser->name << ", would you like to heal your Pokemon post-match? (y/n): ";
            char loserHealChoice;
            cin >> loserHealChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (loserHealChoice == 'y' || loserHealChoice == 'Y') {
                loser->healParty();
            }


        } else {
            Player* pBye = contestants[i];
            cout << "\n" << pBye->name << " gets a bye and advances!" << endl;
            winners.push_back(pBye);
            cout << pBye->name << ", would you like to heal your Pokemon? (y/n): ";
            char healChoice;
            cin >> healChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (healChoice == 'y' || healChoice == 'Y') {
                pBye->healParty();
            }
        }
        if (contestants.size() > 1 && i+2 <= contestants.size()) getIntInput("Press Enter for the next match...",0,0);
    }
    return winners;
}

void startTournament(vector<Player>& allRegisteredPlayers) {
    // clearScreen(); // Optional
    cout << "--- POKEMON TOURNAMENT ---" << std::endl;
    if (allRegisteredPlayers.size() < 2) {
        cout << "Not enough players for a tournament (minimum 2 required)." << endl;
        return;
    }

    vector<Player*> currentContestantsPointers;
    for(Player& p_ref : allRegisteredPlayers) { // Use reference
        currentContestantsPointers.push_back(&p_ref);
    }

    // Part 1: 3 Pokemon Battles
    vector<Player*> winnersPart1 = runTournamentRound(currentContestantsPointers, 3, "Round 1 (3v3)");

    if (winnersPart1.size() <= 1) {
        if (!winnersPart1.empty()) {
            cout << "\n🏆🏆🏆 The Winner of Part 1 is " << winnersPart1[0]->name << "! 🏆🏆🏆" << endl;
            winnersPart1[0]->money += 1000;
            cout << winnersPart1[0]->name << " received $1000!" << endl;
        } else {
            cout << "Part 1 concluded without a clear winner." << endl;
        }
        // For bare minimum, we can end tournament here if only one part is desired or if part 2 is too complex initially.
        // getIntInput("Press Enter to return to menu...",0,0);
        // return;
    }

    // Part 2: 6 Pokemon Battles (only if winners from Part 1 exist and are > 1)
    if (winnersPart1.size() > 1) {
        cout << "\n\n--- Preparing for Part 2: 6v6 Battles! ---" << endl;
        getIntInput("Press Enter to continue...",0,0);
        vector<Player*> finalWinners = runTournamentRound(winnersPart1, 6, "Final Round (6v6)"); // winnersPart1 is already vector<Player*>

        if (!finalWinners.empty() && finalWinners.size() == 1) {
            cout << "\n🏆🏆🏆 The ULTIMATE Grand Winner of the Tournament is " << finalWinners[0]->name << "! 🏆🏆🏆" << endl;
            finalWinners[0]->money += 2500;
            cout << finalWinners[0]->name << " received an additional $2500!" << endl;
        } else {
            cout << "The final round concluded." << (finalWinners.empty() ? " No winner." : " Multiple finalists remain.") << endl;
        }
    } else if (winnersPart1.size() == 1) {
        // Already handled above, this block is just for clarity that Part 2 is skipped.
        cout << "\nTournament concluded after Part 1 as only one winner remained." << endl;
    }


    getIntInput("Press Enter to return to the main menu...",0,0);
}
