#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "player.h"
#include <vector>
#include <string>
using namespace std;

void displayBracket(const vector<Player*>& players, const string& roundName);
vector<Player*> runTournamentRound(vector<Player*>& contestants, int teamSize, const string& roundName);
void startTournament(vector<Player>& allPlayersInGame); // Takes a list of players who will register

#endif // TOURNAMENT_H