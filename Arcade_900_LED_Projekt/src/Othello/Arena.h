#pragma once
// Author Lukas Landgraf

#include "Position.h"
#include "Player.h"
#include "lobby.h"
#include "DLEDController.h"

#define noLED true


class Arena{
    Position ** Field;
    DLEDController * LED;
    uint8_t width;
    uint8_t height;
    Othello_Player * Players;
    uint8_t Bombradius;
    uint8_t MaxPlayer;
    bool Bombphase = false;
    bool Gamefinished = false;
    bool lastCursorWhite = false;
    uint8_t inv = 0;
    Lobby * lobby;
public:
    bool getBombphase(){
        return Bombphase;
    };
    bool getGamefinished(){
        return Gamefinished;
    };
    uint8_t getMaxPlayer(){
        return MaxPlayer;
    };
    uint8_t getHeight(){
        return height;
    };
    uint8_t getWidth(){
        return width;
    };
    Arena(uint8_t, uint8_t, DLEDController *, Lobby *);
    ~Arena();
    void loadMap(uint8_t);
    Position * validPosition(int, int);
    int validMoveAvailable(uint8_t);
    bool validMove(uint8_t, uint8_t, uint8_t, std::vector<Position *> *);
    bool applymove(uint8_t, uint8_t, uint8_t, Player *);
    void PlayerPopUp(uint8_t, Player *);
    void DisplayMap(uint8_t, uint8_t);
    void endGame();
    void setBombphase();
};

