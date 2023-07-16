#pragma once 
// Author Lukas Landgraf
#include "vector"

class Position{
public:
    char type;
    Position ** closePositions;
Position();
~Position();
bool validmoveRecursive(Position*, char, std::vector<Position *> * );
void Bombrecursive(std::vector<Position *> *, uint8_t);
};