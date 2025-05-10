#include "player.h"
#include "utils.h" // For getIntInput
#include <iostream>
#include <algorithm> // For std::find
using namespace std;

void Player::displayPartySummary() const {
    cout << name << "'s Party:" << endl;
    if (pokemonParty.empty()) {
        cout << "  Party is empty." << endl;
        return;
    }
    for (size_t i = 0; i < pokemonParty.size(); ++i) {
        cout << "  " << i + 1 << ". ";
        pokemonParty[i].displaySummary();
        // Indicate if this Pokemon is in the currently selected active team
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
        return true; // Edge case for non-tournament single battles where activeIndices might not be used
    }
    for (int index : activePokemonIndices) {
        if (index >= 0 && static_cast<size_t>(index) < pokemonParty.size() && !pokemonParty[index].isFainted) {
            return true;
        }
    }
    // Also check the explicitly set currentPokemon if activePokemonIndices is not the primary mechanism
    if (currentPokemon && !currentPokemon->isFainted) {
         // Check if currentPokemon is part of the active list or if active list is empty
        bool currentInActive = false;
        for (int idx : activePokemonIndices) {
            if (&pokemonParty[idx] == currentPokemon) {
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
    currentPokemon = getFirstAvailablePokemon(); // Reset current Pokemon if needed
}

Pokemon* Player::getFirstAvailablePokemon() {
    if (activePokemonIndices.empty()) { // Fallback to full party if active indices aren't set
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
    return nullptr; // No Pokemon available
}


void Player::chooseActivePokemonForBattle(int teamSize) {
    activePokemonIndices.clear();
    if (pokemonParty.empty()) {
        cout << name << " has no Pokemon to choose from!" << endl;
        return;
    }

    cout << "\n" << name << ", choose " << teamSize << " Pokemon for your active team:" << endl;
    displayPartySummary();

    vector<int> chosenSoFar; // To prevent choosing the same Pokemon twice

    for (int i = 0; i < teamSize; ++i) {
        if (chosenSoFar.size() >= pokemonParty.size()) {
            cout << "Not enough unique Pokemon to choose from." << endl;
            break;
        }
        int choice = -1;
        bool validInput = false;
        while(!validInput) {
            choice = getIntInput("Choose Pokemon #" + to_string(i + 1) + " (1-" + to_string(pokemonParty.size()) + "): ", 1, pokemonParty.size()) - 1; // 0-indexed

            if (pokemonParty[choice].isFainted) {
                cout << pokemonParty[choice].name << " is fainted and cannot be chosen." << endl;
            } else if (find(chosenSoFar.begin(), chosenSoFar.end(), choice) != chosenSoFar.end()) {
                cout << pokemonParty[choice].name << " has already been chosen for the active team." << endl;
            }
            else {
                validInput = true;
            }
        }
        activePokemonIndices.push_back(choice);
        chosenSoFar.push_back(choice);
        cout << pokemonParty[choice].name << " added to active team." << endl;
    }
     if (!activePokemonIndices.empty()) {
        currentPokemon = &pokemonParty[activePokemonIndices[0]]; // Set the first chosen as current
    } else {
        currentPokemon = nullptr;
    }
}