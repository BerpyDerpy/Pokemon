#include "player.h"
#include "utils.h" // For getIntInput
#include <iostream> 
#include <algorithm> // For find 


void Player::displayPartySummary() const {
    cout << name << "'s Party:" << endl;
    if (pokemonParty.empty()) {
        cout << "  Party is empty." << endl;
        return;
    }
    for (size_t i = 0; i < pokemonParty.size(); ++i) {
        cout << "  " << i + 1 << ". ";
        pokemonParty[i].displaySummary();
        bool isActive = false;
        for(int activeIdx : activePokemonIndices) {
            if(static_cast<size_t>(activeIdx) == i) {
                isActive = true;
                break;
            }
        }
        if(isActive) cout << " [ACTIVE]";
        cout << endl;
    }
}

bool Player::canBattle() const {
    if (activePokemonIndices.empty() && currentPokemon && !currentPokemon->isFainted) {
        return true;
    }
    for (int index : activePokemonIndices) {
        if (index >= 0 && static_cast<size_t>(index) < pokemonParty.size() && !pokemonParty[index].isFainted) {
            return true;
        }
    }
    if (currentPokemon && !currentPokemon->isFainted) {
        bool currentInActive = false;
        for (int idx : activePokemonIndices) {
            if (&pokemonParty[idx] == currentPokemon) { // Check if addresses are the same
                currentInActive = true;
                break;
            }
        }
        if (activePokemonIndices.empty() || currentInActive) return true;
    }
    return false;
}

void Player::healParty() {
    cout << "\nHealing " << name << "'s Pokemon at the Pokemon Center..." << endl;
    for (Pokemon& pkm : pokemonParty) {
        pkm.heal();
    }
    currentPokemon = getFirstAvailablePokemon();
}

Pokemon* Player::getFirstAvailablePokemon() {
    if (activePokemonIndices.empty()) {
        for (size_t i = 0; i < pokemonParty.size(); ++i) {
            if (!pokemonParty[i].isFainted) {
                return &pokemonParty[i];
            }
        }
    } else {
        for (int index : activePokemonIndices) {
            if (index >= 0 && static_cast<size_t>(index) < pokemonParty.size() && !pokemonParty[index].isFainted) {
                return &pokemonParty[index];
            }
        }
    }
    return nullptr;
}

void Player::chooseActivePokemonForBattle(int teamSize) {
    activePokemonIndices.clear();
    if (pokemonParty.empty()) {
        cout << name << " has no Pokemon to choose from!" << endl;
        return;
    }

    cout << "\n" << name << ", choose " << teamSize << " Pokemon for your active team:" << endl;
    displayPartySummary(); // Show full party, not just active ones, for selection

    vector<int> chosenSoFarIndices; // Store indices from pokemonParty already picked

    for (int i = 0; i < teamSize; ++i) {
        if (chosenSoFarIndices.size() >= pokemonParty.size() && i < teamSize) {
             cout << "Not enough unique, non-fainted Pokemon to form a full team of " << teamSize << "." << endl;
             break;
        }
        int choice_idx = -1; // This will be the 0-based index into pokemonParty
        bool validInput = false;
        while(!validInput) {
            // User input is 1-based
            int user_choice = getIntInput("Choose Pokemon #" + to_string(i + 1) + " (1-" + to_string(pokemonParty.size()) + "): ", 1, pokemonParty.size());
            choice_idx = user_choice - 1; // Convert to 0-based index

            if (pokemonParty[choice_idx].isFainted) {
                cout << pokemonParty[choice_idx].name << " is fainted and cannot be chosen." << endl;
            } else if (find(chosenSoFarIndices.begin(), chosenSoFarIndices.end(), choice_idx) != chosenSoFarIndices.end()) {
                cout << pokemonParty[choice_idx].name << " has already been chosen for the active team." << endl;
            } else {
                validInput = true;
            }
        }
        activePokemonIndices.push_back(choice_idx); // Store the 0-based index from pokemonParty
        chosenSoFarIndices.push_back(choice_idx);
        cout << pokemonParty[choice_idx].name << " added to active team." << endl;
    }
     if (!activePokemonIndices.empty()) {
        // currentPokemon should point to an element within pokemonParty using the stored index
        currentPokemon = &pokemonParty[activePokemonIndices[0]];
    } else {
        currentPokemon = nullptr; // No active Pokemon selected
    }
}
