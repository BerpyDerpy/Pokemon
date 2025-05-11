#include "battle_logic.h"
#include "utils.h"     // For getRandomInt, getIntInput, clearScreen
#include "game_data.h" // For TYPE_CHART
#include <iostream>    // For cout, endl
#include <string>      // For string
#include <algorithm>   // For max, find (std::max becomes max)
#include <vector>      // For vector (used in AI part)

float getTypeEffectiveness(string& moveType, string& defenderType1, string& defenderType2) {
    float effectiveness = 1.0f;

    if (TYPE_CHART.count(moveType)) { // Check if attacking type exists in chart
        auto& defending_map = TYPE_CHART.at(moveType); // Use .at for bounds checking (throws exception if not found)
        if (defending_map.count(defenderType1)) {
            effectiveness *= defending_map.at(defenderType1);
        }
        if (!defenderType2.empty() && defending_map.count(defenderType2)) {
            effectiveness *= defending_map.at(defenderType2);
        }
    }
    return effectiveness;
}

int calculateDamage( Move& move,Pokemon& attacker, Pokemon& defender) {
    if (move.minDamage == 0 && move.maxDamage == 0) return 0; // Status move

    int baseDamage = getRandomInt(move.minDamage, move.maxDamage);
    float typeMultiplier = getTypeEffectiveness(move.type, defender.type1, defender.type2);

    int finalDamage = static_cast<int>(baseDamage * typeMultiplier);

    if (typeMultiplier > 1.5f) cout << "It's super effective!" << endl;
    if (typeMultiplier < 0.75f && typeMultiplier > 0.0f) cout << "It's not very effective..." << endl;
    if (typeMultiplier <= 0.0f) cout << defender.name << " is immune!" << endl; // Changed to <= 0.0f for clarity

    return std::max(0, finalDamage); // Damage can be 0 if immune, or 1 if not very effective but still hits
                                     // Pokemon tradition often has minimum 1 damage unless immune. Let's stick to that.
                                     // So if finalDamage < 1 and typeMultiplier > 0, make it 1.
    if (finalDamage < 1 && typeMultiplier > 0.0f) return 1;
    return finalDamage < 0 ? 0 : finalDamage; // ensure non-negative
}

bool attemptDodge( Pokemon& defender, int dodgeChancePercent) {
    if (getRandomInt(1, 100) <= dodgeChancePercent) {
        cout << defender.name << " dodged the attack!" << endl;
        return true;
    }
    return false;
}

void executeTurn(Player& attackerPlayer, Player& defenderPlayer, int moveChoiceIndex) {
    if (!attackerPlayer.currentPokemon || attackerPlayer.currentPokemon->isFainted) {
        cout << attackerPlayer.name << "'s current Pokemon can't attack!" << endl; // Simplified message
        return;
    }
    if (!defenderPlayer.currentPokemon || defenderPlayer.currentPokemon->isFainted) {
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
        return;
    }

    int damageDealt = calculateDamage(chosenMove, *(attackerPlayer.currentPokemon), *(defenderPlayer.currentPokemon));
    defenderPlayer.currentPokemon->currentHP -= damageDealt;

    cout << chosenMove.name << " dealt " << damageDealt << " damage to " << defenderPlayer.currentPokemon->name << "." << endl;

    if (defenderPlayer.currentPokemon->currentHP <= 0) {
        defenderPlayer.currentPokemon->currentHP = 0;
        defenderPlayer.currentPokemon->isFainted = true;
        // cout << defenderPlayer.currentPokemon->name << " fainted!" << endl; // This is printed in startBattle
    } else {
        cout << defenderPlayer.currentPokemon->name << " has " << defenderPlayer.currentPokemon->currentHP << " HP remaining." << endl;
    }
}

Player* startBattle(Player& player1, Player& player2, bool isGymBattle) {
    cout << "\n--- BATTLE START ---" << endl;
    cout << player1.name << " vs " << player2.name << endl;

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

    Player* currentPlayer = &player1;
    Player* waitingPlayer = &player2;

    while (player1.canBattle() && player2.canBattle()) {
        clearScreen(); // Optional: can make it jumpy, enable if preferred
        cout << "\n--- " << currentPlayer->name << "'s Turn ---" << endl;
        if (!currentPlayer->currentPokemon) { // Should not happen if logic is correct
            cout << currentPlayer->name << " has no active Pokemon!" << endl; break;
        }
        currentPlayer->currentPokemon->displayDetailed();
        cout << "\n--- Opponent ---" << endl;
        if (!waitingPlayer->currentPokemon) { // Should not happen
             cout << waitingPlayer->name << " has no active Pokemon!" << endl; break;
        }
        waitingPlayer->currentPokemon->displaySummary();
        cout << "\n----------------\n" << endl;

        int choice = -1; // For move selection
        int actionChoice = 1; // Default to fight for AI or if switch fails

        if (isGymBattle && currentPlayer == &player2) {
            cout << currentPlayer->name << " (Gym Leader) is thinking..." << endl;
            vector<int> availableMoveIndices;
            for(size_t i = 0; i < currentPlayer->currentPokemon->moveset.size(); ++i) {
                if(currentPlayer->currentPokemon->moveset[i].currentPP > 0) {
                    availableMoveIndices.push_back(i);
                }
            }
            if (!availableMoveIndices.empty()) {
                choice = availableMoveIndices[getRandomInt(0, availableMoveIndices.size() -1)] + 1;
                cout << currentPlayer->name << " chose to attack with " << currentPlayer->currentPokemon->moveset[choice-1].name << endl;
            } else {
                cout << currentPlayer->currentPokemon->name << " has no PP left! (Struggle not implemented)" << endl;
                choice = 0; // Effectively do nothing, skip turn
            }
        } else {
            cout << "Choose an action for " << currentPlayer->currentPokemon->name << ":" << endl;
            cout << "1. Fight" << endl;
            cout << "2. Switch Pokemon" << endl;
            actionChoice = getIntInput("Enter choice (1-2): ", 1, 2);

            if (actionChoice == 1) { // Fight
                cout << "Choose a move:" << endl;
                for (size_t i = 0; i < currentPlayer->currentPokemon->moveset.size(); ++i) {
                    cout << i + 1 << ". ";
                    currentPlayer->currentPokemon->moveset[i].displayMove();
                    cout << endl;
                }
                choice = getIntInput("Enter move choice (1-" + to_string(currentPlayer->currentPokemon->moveset.size()) + ", or 0 to go back): ", 0, currentPlayer->currentPokemon->moveset.size());
                if (choice == 0) continue;
            } else if (actionChoice == 2) { // Switch Pokemon
                 if (isGymBattle && currentPlayer == &player2) {
                    cout << "Gym Leaders cannot switch Pokemon voluntarily!" << endl;
                    actionChoice = 1; // Force AI to fight if switch is chosen
                    // AI will pick a move in the next block if actionChoice is 1
                } else {
                    cout << "Choose a Pokemon to switch to from your PARTY (not just active tournament team):" << endl;
                    currentPlayer->displayPartySummary(); // Show entire party for switching
                    vector<int> availableSwitchPartyIndices; // Indices from the main pokemonParty
                    for(size_t i=0; i < currentPlayer->pokemonParty.size(); ++i) {
                        if(!currentPlayer->pokemonParty[i].isFainted && &currentPlayer->pokemonParty[i] != currentPlayer->currentPokemon) {
                            availableSwitchPartyIndices.push_back(i);
                            // Displaying with a new 1-based index for user selection:
                            cout << availableSwitchPartyIndices.size() << ". " << currentPlayer->pokemonParty[i].name << endl;
                        }
                    }

                    if (availableSwitchPartyIndices.empty()) {
                        cout << "No other Pokemon available to switch to!" << endl;
                        actionChoice = 1; // Force fight if no switch available
                    } else {
                        int switchInput = getIntInput("Enter Pokemon to switch to (1-" + to_string(availableSwitchPartyIndices.size()) + ", or 0 to cancel): ", 0, availableSwitchPartyIndices.size());
                        if (switchInput > 0) {
                            int partyIdxToSwitchTo = availableSwitchPartyIndices[switchInput - 1];
                            currentPlayer->currentPokemon = &currentPlayer->pokemonParty[partyIdxToSwitchTo];
                            // Update activePokemonIndices if this newly selected Pokemon is not in it,
                            // or if activePokemonIndices is being strictly managed. For simplicity now,
                            // we directly switch currentPokemon. In tournaments, this needs careful handling.
                            cout << currentPlayer->name << " switched to " << currentPlayer->currentPokemon->name << "!" << endl;
                            currentPlayer->currentPokemon->displayDetailed();
                            swap(currentPlayer, waitingPlayer); // Switch turn
                            getIntInput("\nPress Enter to continue...",0,0);
                            continue; // Skip to next player's turn
                        } else {
                            actionChoice = 1; // Cancelled switch, so fight
                        }
                    }
                }
                 if (actionChoice == 1 && (isGymBattle && currentPlayer == &player2)) { // Gym leader forced to fight after trying to switch
                    vector<int> availableMoveIndices;
                    for(size_t i = 0; i < currentPlayer->currentPokemon->moveset.size(); ++i) {
                        if(currentPlayer->currentPokemon->moveset[i].currentPP > 0) availableMoveIndices.push_back(i);
                    }
                    if (!availableMoveIndices.empty()) {
                        choice = availableMoveIndices[getRandomInt(0, availableMoveIndices.size() -1)] + 1;
                    } else choice = 0;
                }
            }
        }

        if (choice > 0 && choice <= static_cast<int>(currentPlayer->currentPokemon->moveset.size())) {
            executeTurn(*currentPlayer, *waitingPlayer, choice - 1);
        } else if (choice == 0 && actionChoice == 1) { // Chose "Fight" then "Go Back" or AI has no moves
            cout << currentPlayer->name << " didn't make a move." << endl;
        } else if (actionChoice != 2) { // If not a successful switch
             cout << "Invalid move choice or action." << endl;
        }


        if (waitingPlayer->currentPokemon && waitingPlayer->currentPokemon->isFainted) {
            cout << "\n" << waitingPlayer->currentPokemon->name << " fainted!" << endl;
            if (!waitingPlayer->canBattle()) { // canBattle checks activePokemonIndices.
                                              // For a general battle, it should check pokemonParty if activeIndices is empty.
                                              // Modifying canBattle for this scenario.
                Pokemon* next = waitingPlayer->getFirstAvailablePokemon(); // Check general party
                if (!next) {
                     cout << waitingPlayer->name << " has no more Pokemon that can battle!" << endl;
                     break;
                } else { // This else means canBattle() was too restrictive, force switch from general party
                     cout << waitingPlayer->name << ", choose your next Pokemon:" << endl;
                    // Force switch logic (simplified)
                     waitingPlayer->displayPartySummary();
                    vector<int> availableSwitchPartyIndices;
                    for(size_t i=0; i< waitingPlayer->pokemonParty.size(); ++i) {
                        if(!waitingPlayer->pokemonParty[i].isFainted) {
                            availableSwitchPartyIndices.push_back(i);
                            cout << availableSwitchPartyIndices.size() << ". " << waitingPlayer->pokemonParty[i].name << endl;
                        }
                    }
                     if (isGymBattle && waitingPlayer == &player2) {
                        if(!availableSwitchPartyIndices.empty()) waitingPlayer->currentPokemon = &waitingPlayer->pokemonParty[availableSwitchPartyIndices[0]];
                    } else {
                        if(!availableSwitchPartyIndices.empty()){
                            int switchInput = getIntInput("Enter Pokemon to send out (1-" + to_string(availableSwitchPartyIndices.size()) + "): ", 1, availableSwitchPartyIndices.size());
                            waitingPlayer->currentPokemon = &waitingPlayer->pokemonParty[availableSwitchPartyIndices[switchInput - 1]];
                        } else { /* Should be caught by 'next' check above */ }
                    }
                    cout << waitingPlayer->name << " sends out " << waitingPlayer->currentPokemon->name << "!" << endl;
                    waitingPlayer->currentPokemon->displayDetailed();
                }

            } else { // canBattle() is true, meaning there are switch options within the active team or general party
                cout << waitingPlayer->name << ", choose your next Pokemon:" << endl;
                // Logic to select next Pokemon (simplified for now, AI picks first, player prompted)
                if (isGymBattle && waitingPlayer == &player2) { // Gym Leader AI forced switch
                    waitingPlayer->currentPokemon = waitingPlayer->getFirstAvailablePokemon(); // AI sends out next available from its list
                } else { // Human player forced switch
                    waitingPlayer->displayPartySummary(); // Show party
                    vector<int> availableSwitchPartyIndices; // Indices from pokemonParty
                    for(size_t i=0; i< waitingPlayer->pokemonParty.size(); ++i) {
                        // If using activePokemonIndices strictly, filter by that list. Otherwise, any non-fainted.
                        bool inActiveList = waitingPlayer->activePokemonIndices.empty(); // if active list empty, any party member is fine
                        if (!inActiveList) {
                            for(int activeIdx : waitingPlayer->activePokemonIndices) if(static_cast<size_t>(activeIdx) == i) inActiveList = true;
                        }

                        if(inActiveList && !waitingPlayer->pokemonParty[i].isFainted) {
                            availableSwitchPartyIndices.push_back(i);
                             cout << availableSwitchPartyIndices.size() << ". " << waitingPlayer->pokemonParty[i].name << endl;
                        }
                    }
                     if (!availableSwitchPartyIndices.empty()) {
                        int switchInput = getIntInput("Enter Pokemon to send out (1-" + to_string(availableSwitchPartyIndices.size()) + "): ", 1, availableSwitchPartyIndices.size());
                        waitingPlayer->currentPokemon = &waitingPlayer->pokemonParty[availableSwitchPartyIndices[switchInput - 1]];
                    } else {
                        // This should be caught by canBattle more gracefully. If we reach here, it's an issue.
                        cout << "Error: No Pokemon to switch to, but canBattle() was true." << endl; break;
                    }
                }
                if(waitingPlayer->currentPokemon) {
                    cout << waitingPlayer->name << " sends out " << waitingPlayer->currentPokemon->name << "!" << endl;
                    waitingPlayer->currentPokemon->displayDetailed();
                } else {
                     cout << waitingPlayer->name << " has no more Pokemon to send out!" << endl; break;
                }
            }
        }

        swap(currentPlayer, waitingPlayer);
        if (choice !=0 || actionChoice == 2) { // Pause if an action (fight/successful switch) was taken
            getIntInput("\nPress Enter to continue...",0,0);
        }
    } // End of while battle loop

    // clearScreen(); // Optional
    // Check who won based on whose Pokemon are all fainted
    bool p1_can_battle = false;
    for(const auto& pkm : player1.pokemonParty) if(!pkm.isFainted) p1_can_battle = true;
    bool p2_can_battle = false;
    for(const auto& pkm : player2.pokemonParty) if(!pkm.isFainted) p2_can_battle = true;


    if (p1_can_battle && !p2_can_battle) {
        cout << player2.name << " is out of usable Pokemon!" << endl;
        cout << player1.name << " wins the battle!" << endl;
        player1.money += 200;
        cout << player1.name << " got $200 for winning!" << endl;
        return &player1;
    } else if (p2_can_battle && !p1_can_battle) {
        cout << player1.name << " is out of usable Pokemon!" << endl;
        cout << player2.name << " wins the battle!" << endl;
        player2.money += 200;
        cout << player2.name << " got $200 for winning!" << endl;
        return &player2;
    } else if (!p1_can_battle && !p2_can_battle) {
        cout << "Both players are out of usable Pokemon! It's a draw!" << endl;
        return nullptr;
    }
    else { // Should ideally not be reached if loop condition and win checks are correct
        cout << "The battle ended inconclusively." << endl;
        // Check currentPokemon status as a fallback
        if (player1.currentPokemon && !player1.currentPokemon->isFainted && (!player2.currentPokemon || player2.currentPokemon->isFainted)) return &player1;
        if (player2.currentPokemon && !player2.currentPokemon->isFainted && (!player1.currentPokemon || player1.currentPokemon->isFainted)) return &player2;
        return nullptr;
    }
}
