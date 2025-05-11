#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "player.h" // Includes vector, string
using namespace std; 

void displayBracket(const vector<Player*>& players, const string& roundName);
vector<Player*> runTournamentRound(vector<Player*>& contestants, int teamSize, const string& roundName);
void startTournament(vector<Player>& allPlayersInGame);

#endif // TOURNAMENT_H
