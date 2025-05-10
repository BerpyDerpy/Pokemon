#include "battle_logic.h"
#include "utils.h" // For getRandomInt, getIntInput, clearScreen
#include "game_data.h" // For TYPE_CHART
#include <iostream>
#include <string>
#include <algorithm> // for max
using namespace std;

float getTypeEffectiveness(const string& moveType, const string& defenderType1, const string& defenderType2) {
    float effectiveness = 1.0f;

    // Check against type1
    if (TYPE_CHART.count(moveType) && TYPE_CHART[moveType].count(defenderType1)) {
        effectiveness *= TYPE_CHART[moveType][defenderType1];
    }

    // Check against type2 if it exists
    if (!defenderType2.empty() && TYPE_CHART.count(moveType) && TYPE_CHART[moveType].count(defenderType2)) {
        effectiveness *= TYPE_CHART[moveType][defenderType2];
    }
    return effectiveness;
}

int calculateDamage(const Move& move, const Pokemon& attacker, const Pokemon& defender) {
    if (move.minDamage == 0 && move.maxDamage == 0) return 0; // Status move

    int baseDamage = getRandomInt(move.minDamage, move.maxDamage);
    float typeMultiplier = getTypeEffectiveness(move.type, defender.type1, defender.type2);

    // Could add STAB (Same Type Attack Bonus) here: if (move.type == attacker.type1 || move.type == attacker.type2) multiplier *= 1.5;
    // Could add critical hit chance here

    int finalDamage = static_cast<int>(baseDamage * typeMultiplier);

    if (typeMultiplier > 1.5f) cout << "It's super effective!" << endl;
    if (typeMultiplier < 0.75f && typeMultiplier > 0.0f) cout << "It's not very effective..." << endl;
    if (typeMultiplier == 0.0f) cout << defender.name << " is immune!" << endl;

    return max(1, finalDamage); // Ensure at least 1 damage if not immune and not a status move
}

bool attemptDodge(const Pokemon& defender, int dodgeChancePercent) {
    // Pokemon specific stats could influence dodgeChancePercent later (e.g., speed, agility)
    if (getRandomInt(1, 100) <= dodgeChancePercent) {
        cout << defender.name << " dodged the attack!" << endl;
        return true;
    }
    return false;
}

void executeTurn(Player& attackerPlayer, Player& defenderPlayer, int moveChoiceIndex) {
    if (!attackerPlayer.currentPokemon || attackerPlayer.currentPokemon->isFainted) {
        cout << attackerPlayer.name << "'s " << attackerPlayer.currentPokemon->name << " can't attack!" << endl;
        return;
    }
    if (!defenderPlayer.currentPokemon || defenderPlayer.currentPokemon->isFainted) {
        // This shouldn't happen if battle logic is correct, defender should have been forced to switch
        cout << "Defender's Pokemon has already fainted." << endl;
        return;
    }


    Move& chosenMove = attackerPlayer.currentPokemon->moveset[moveChoiceIndex];

    cout << "\n" << attackerPlayer.currentPokemon->name << " uses " << chosenMove.name << "!" << endl;

    if (chosenMove.currentPP <= 0) {
        cout << "But there's no PP left for this move!" << endl;
        return;
    }
    chosenMove.currentPP--;

    if (attemptDodge(*(defenderPlayer.currentPokemon))) {
        return; // Attack missed
    }

    int damageDealt = calculateDamage(chosenMove, *(attackerPlayer.currentPokemon), *(defenderPlayer.currentPokemon));
    defenderPlayer.currentPokemon->currentHP -= damageDealt;

    cout << chosenMove.name << " dealt " << damageDealt << " damage to " << defenderPlayer.currentPokemon->name << "." << endl;

    if (defenderPlayer.currentPokemon->currentHP <= 0) {
        defenderPlayer.currentPokemon->currentHP = 0;
        defenderPlayer.currentPokemon->isFainted = true;
        cout << defenderPlayer.currentPokemon->name << " fainted!" << endl;
    } else {
        cout << defenderPlayer.currentPokemon->name << " has " << defenderPlayer.currentPokemon->currentHP << " HP remaining." << endl;
    }
}

Player* startBattle(Player& player1, Player& player2, bool isGymBattle) {
    cout << "\n--- BATTLE START ---" << endl;
    cout << player1.name << " vs " << player2.name << endl;

    // Ensure players have active Pokemon selected, or select their first available
    if (!player1.currentPokemon || player1.currentPokemon->isFainted) player1.currentPokemon = player1.getFirstAvailablePokemon();
    if (!player2.currentPokemon || player2.currentPokemon->isFainted) player2.currentPokemon = player2.getFirstAvailablePokemon();


    if (!player1.currentPokemon) {
        cout << player1.name << " has no usable Pokemon! " << player2.name << " wins by default!" << endl;
        return &player2;
    }
    if (!player2.currentPokemon) {
        cout << player2.name << " has no usable Pokemon! " << player1.name << " wins by default!" << endl;
        return &player1;
    }

    cout << player1.name << " sends out " << player1.currentPokemon->name << "!" << endl;
    player1.currentPokemon->displayDetailed();
    cout << player2.name << " sends out " << player2.currentPokemon->name << "!" << endl;
    player2.currentPokemon->displayDetailed();

    Player* currentPlayer = &player1; // Player 1 starts
    Player* waitingPlayer = &player2;

    while (player1.canBattle() && player2.canBattle()) {
        clearScreen(); // Clear screen for new turn info
        cout << "\n--- " << currentPlayer->name << "'s Turn ---" << endl;
        currentPlayer->currentPokemon->displayDetailed();
        cout << "\n--- Opponent ---" << endl;
        waitingPlayer->currentPokemon->displaySummary(); // Show summary of opponent
        cout << "\n----------------\n" << endl;


        int choice = -1;
        if (isGymBattle && currentPlayer == &player2) { // Simple AI for Gym Leader (Player 2)
            cout << currentPlayer->name << " (Gym Leader) is thinking..." << endl;
            // AI: Choose a random available move
            vector<int> availableMoves;
            for(size_t i = 0; i < currentPlayer->currentPokemon->moveset.size(); ++i) {
                if(currentPlayer->currentPokemon->moveset[i].currentPP > 0) {
                    availableMoves.push_back(i);
                }
            }
            if (!availableMoves.empty()) {
                choice = availableMoves[getRandomInt(0, availableMoves.size() -1)] + 1; // 1-indexed choice
                cout << currentPlayer->name << " chose to attack with " << currentPlayer->currentPokemon->moveset[choice-1].name << endl;
            } else {
                cout << currentPlayer->currentPokemon->name << " has no PP left for any move! (Struggle not implemented)" << endl;
                // TODO: Implement Struggle
                choice = 0; // Do nothing effectively
            }

        } else { // Human player's turn
            cout << "Choose an action for " << currentPlayer->currentPokemon->name << ":" << endl;
            cout << "1. Fight" << endl;
            cout << "2. Switch Pokemon" << endl;
            // cout << "3. Use Item (Not Implemented)" << endl;
            // cout << "4. Run (Not Implemented for trainer battles)" << endl;

            int actionChoice = getIntInput("Enter choice (1-2): ", 1, 2);

            if (actionChoice == 1) { // Fight
                cout << "Choose a move:" << endl;
                for (size_t i = 0; i < currentPlayer->currentPokemon->moveset.size(); ++i) {
                    cout << i + 1 << ". ";
                    currentPlayer->currentPokemon->moveset[i].displayMove();
                    cout << endl;
                }
                choice = getIntInput("Enter move choice (1-" + to_string(currentPlayer->currentPokemon->moveset.size()) + ", or 0 to go back): ", 0, currentPlayer->currentPokemon->moveset.size());
                if (choice == 0) continue; // Go back to action selection
                // choice is 1-indexed from input, convert to 0-indexed for moveset vector
            } else if (actionChoice == 2) { // Switch Pokemon
                if (isGymBattle && currentPlayer == &player2) {
                    cout << "Gym Leaders cannot switch Pokemon voluntarily!" << endl;
                    continue; // Gym leader must attack or if forced to switch, it's handled below
                }
                cout << "Choose a Pokemon to switch to:" << endl;
                currentPlayer->displayPartySummary(); // Shows full party
                vector<int> availableSwitchIndices; // Actual indices in pokemonParty
                for(size_t i=0; i< currentPlayer->pokemonParty.size(); ++i) {
                    // Check if this pokemon is in the active list for tournament/specific battles
                    bool inActiveTeam = false;
                    if (currentPlayer->activePokemonIndices.empty()) { // If not using activePokemonIndices (e.g. simple 1v1)
                        inActiveTeam = true;
                    } else {
                        for(int activeIdx : currentPlayer->activePokemonIndices) {
                            if (static_cast<size_t>(activeIdx) == i) {
                                inActiveTeam = true;
                                break;
                            }
                        }
                    }

                    if(inActiveTeam && !currentPlayer->pokemonParty[i].isFainted && &currentPlayer->pokemonParty[i] != currentPlayer->currentPokemon) {
                        availableSwitchIndices.push_back(i);
                        cout << availableSwitchIndices.size() << ". " << currentPlayer->pokemonParty[i].name << endl;
                    }
                }

                if (availableSwitchIndices.empty()) {
                    cout << "No other Pokemon available to switch to!" << endl;
                    continue;
                }

                int switchChoice = getIntInput("Enter Pokemon to switch to (1-" + to_string(availableSwitchIndices.size()) + ", or 0 to cancel): ", 0, availableSwitchIndices.size());
                if (switchChoice > 0) {
                    currentPlayer->currentPokemon = &currentPlayer->pokemonParty[availableSwitchIndices[switchChoice - 1]];
                    cout << currentPlayer->name << " switched to " << currentPlayer->currentPokemon->name << "!" << endl;
                    currentPlayer->currentPokemon->displayDetailed();
                    // Switch turn after switching Pokemon
                    swap(currentPlayer, waitingPlayer);
                    continue;
                } else {
                    continue; // Cancelled switch
                }
            }
        }


        if (choice > 0 && choice <= static_cast<int>(currentPlayer->currentPokemon->moveset.size())) {
            executeTurn(*currentPlayer, *waitingPlayer, choice - 1); // choice-1 for 0-indexed move
        } else if (choice != 0) { // choice 0 means did nothing or cancelled
             cout << "Invalid move choice or action." << endl;
        }


        // Check if waiting player's Pokemon fainted
        if (waitingPlayer->currentPokemon && waitingPlayer->currentPokemon->isFainted) {
            cout << waitingPlayer->name << "'s " << waitingPlayer->currentPokemon->name << " fainted!" << endl;
            if (!waitingPlayer->canBattle()) {
                cout << waitingPlayer->name << " has no more Pokemon that can battle!" << endl;
                break; // Battle ends
            } else {
                cout << waitingPlayer->name << ", choose your next Pokemon:" << endl;
                 waitingPlayer->displayPartySummary();
                vector<int> availableSwitchIndices;
                for(size_t i=0; i< waitingPlayer->pokemonParty.size(); ++i) {
                    bool inActiveTeam = false;
                     if (waitingPlayer->activePokemonIndices.empty()) {
                        inActiveTeam = true;
                    } else {
                        for(int activeIdx : waitingPlayer->activePokemonIndices) {
                            if (static_cast<size_t>(activeIdx) == i) {
                                inActiveTeam = true;
                                break;
                            }
                        }
                    }
                    if(inActiveTeam && !waitingPlayer->pokemonParty[i].isFainted) {
                        availableSwitchIndices.push_back(i);
                        cout << availableSwitchIndices.size() << ". " << waitingPlayer->pokemonParty[i].name << endl;
                    }
                }

                if (isGymBattle && waitingPlayer == &player2) { // Gym Leader AI for forced switch
                    if (!availableSwitchIndices.empty()) {
                        waitingPlayer->currentPokemon = &waitingPlayer->pokemonParty[availableSwitchIndices[0]]; // Gym leader sends out next available
                        cout << waitingPlayer->name << " sends out " << waitingPlayer->currentPokemon->name << "!" << endl;
                    }
                } else { // Human player forced switch
                    int switchChoiceIdx = getIntInput("Enter Pokemon to send out (1-" + to_string(availableSwitchIndices.size()) + "): ", 1, availableSwitchIndices.size());
                    waitingPlayer->currentPokemon = &waitingPlayer->pokemonParty[availableSwitchIndices[switchChoiceIdx - 1]];
                    cout << waitingPlayer->name << " sends out " << waitingPlayer->currentPokemon->name << "!" << endl;
                }
                waitingPlayer->currentPokemon->displayDetailed();
            }
        }

        // Switch turns
        swap(currentPlayer, waitingPlayer);
        if (choice !=0 ) { // only pause if an action was taken
            getIntInput("\nPress Enter to continue to the next turn...", 0, 0); // Pauses, expects any number then enter, effectively just enter
        }

    } // End of while battle loop

    clearScreen();
    if (player1.canBattle() && !player2.canBattle()) {
        cout << player2.name << " is out of usable Pokemon!" << endl;
        cout << player1.name << " wins the battle!" << endl;
        player1.money += 200; // Winner gets money
        cout << player1.name << " got $200 for winning!" << endl;
        return &player1;
    } else if (player2.canBattle() && !player1.canBattle()) {
        cout << player1.name << " is out of usable Pokemon!" << endl;
        cout << player2.name << " wins the battle!" << endl;
        player2.money += 200;
        cout << player2.name << " got $200 for winning!" << endl;
        return &player2;
    } else {
        // This case should ideally not be reached if logic is perfect,
        // but could be a draw if both run out simultaneously (e.g. recoil, status)
        cout << "The battle ended in a draw or an unexpected state." << endl;
        return nullptr;
    }
}