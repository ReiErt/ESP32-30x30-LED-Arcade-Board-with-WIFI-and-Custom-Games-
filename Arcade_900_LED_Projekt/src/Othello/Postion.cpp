// Author Lukas Landgraf

#include "Position.h"


Position::Position(){
    closePositions = new Position*[8];
}
Position::~Position(){
    delete[] closePositions;
}
// überprüfe ob am Ende eine geraden Linie aus Gegnern ein eigener Stein liegt
bool Position::validmoveRecursive(Position * lastpos, char PlayerID, std::vector<Position *> * conqueredPositions){
    // Zug invalide da am Ende der Linie kein eigener Stein
    if(type == '-' || type == 'b' || type == 'i' || type == 'c' || type == '0'){
        return false;
    }
    // zug Valide 
    else if(type == PlayerID){
        return true;
    }
    // weiter zum nächsten Feld
    else{
        int j;
        for(j = 0; closePositions[j] != lastpos; j++);
        j = (j + 4) % 8;
        if(closePositions[j] != nullptr){
            if(closePositions[j]->validmoveRecursive(this, PlayerID, conqueredPositions)){
                if(conqueredPositions != nullptr){
                    conqueredPositions->push_back(this);
                }
                return true;
            }
        }
        return false;
    }
}
// Finde alle Felder die gesprengt werden
void Position::Bombrecursive(std::vector<Position *> * conqueredPositions, uint8_t step){
    // Bomben Limit erreicht
    if(step == 0){
        return;
    }
    for(uint8_t i = 0; i < 8; i++){
        if(closePositions[i] == nullptr || closePositions[i]->type == '-'){
            continue;
        }
        else{
            closePositions[i]->Bombrecursive(conqueredPositions, step - 1);
            if(conqueredPositions != nullptr){
                conqueredPositions->push_back(this);
            }
        }
    }
}