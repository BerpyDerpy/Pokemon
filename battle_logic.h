#ifndef BATTLE_LOGIC_H
#define BATTLE_LOGIC_H

#include "player.h" // Battle involves players
#include "move.h"
#include "pokemon.h"
#include <map>

// Calculate type effectiveness multiplier
float getTypeEffectiveness(const std::string& moveType, const std::string& defenderType1, const std::string& defenderType2);

// Calculate damage
int calculateDamage(const Move& move, const Pokemon& attacker, const Pokemon& defender);

// Attempt dodge
bool attemptDodge(const Pokemon& defender, int dodgeChancePercent = 10); // e.g., 10% base dodge chance

// Core battle turn execution
void executeTurn(Player& attackerPlayer, Player& defenderPlayer, int moveChoice);

// Main battle loop for two players
Player* startBattle(Player& player1, Player& player2, bool isGymBattle = false); // Returns pointer to the winning player or nullptr for a draw

#endif // BATTLE_LOGIC_H