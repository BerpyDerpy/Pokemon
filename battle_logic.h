#ifndef BATTLE_LOGIC_H
#define BATTLE_LOGIC_H

#include "player.h" // Battle involves players (also brings in pokemon.h, move.h)
#include <map>     
using namespace std;
// Calculate type effectiveness multiplier
float getTypeEffectiveness( string& moveType, string& defenderType1, string& defenderType2);

// Calculate damage
int calculateDamage( Move& move, Pokemon& attacker, Pokemon& defender);

// Attempt dodge
bool attemptDodge( Pokemon& defender, int dodgeChancePercent = 10);

// Core battle turn execution
void executeTurn(Player& attackerPlayer, Player& defenderPlayer, int moveChoice);

// Main battle loop for two players
Player* startBattle(Player& player1, Player& player2, bool isGymBattle = false);

#endif // BATTLE_LOGIC_H
