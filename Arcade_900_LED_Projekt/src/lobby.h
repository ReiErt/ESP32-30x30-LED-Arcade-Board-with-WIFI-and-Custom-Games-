#pragma once
// Author Lukas Landgraf
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <esp_http_server.h>
#include "DLEDController.h"

#define Up 0
#define Left 1
#define Down 2
#define Right 3
#define A 4
#define B 5
#define Select 6
#define Start 7


enum LobbyState{
    lobby_closed,
    lobby_open,
    game_running
};
class Button{
    SemaphoreHandle_t ButtonS = nullptr;
    bool button = false; 
public:
    Button();
    ~Button();
    bool getButton();
    void setButton(bool);
};
class Player{
    bool connected = false;
    SemaphoreHandle_t connectedS = nullptr;
    int sockfd = 0;
    SemaphoreHandle_t sockfdS = nullptr;
public:
    Button Buttons[20];
    Player();
    ~Player();
    bool getconnected();
    void setconnected(bool);
    int getsockfd();
    void setsockfd(int);
};
class Lobby{
    LobbyState state = lobby_closed;
    SemaphoreHandle_t StateS = nullptr;
    SemaphoreHandle_t GameIDS = nullptr;
    uint8_t GameID = 0;
    uint8_t MaxPlayer = 0;
    uint8_t PlayerConected = 0;
    SemaphoreHandle_t PlayersS = nullptr;
    Player Players[4];
    SemaphoreHandle_t ButtonModeS = nullptr;
    bool ButtonMode = true;
    SemaphoreHandle_t ServerHandleS = nullptr;
    httpd_handle_t  ServerHandle = nullptr;
    uint8_t GameData = 0;
    SemaphoreHandle_t GameDataS = nullptr;
public:
    Lobby();
    ~Lobby();
    LobbyState getState();
    void setState(LobbyState);
    uint8_t getPlayerConected();
    void reducePlayercount();
    void setMaxPlayer(uint8_t);
    uint8_t getMaxPlayer();
    void setGameID(uint8_t);
    uint8_t getGameID();
    bool addPlayer(httpd_req_t *);
    Player * getPlayer(uint8_t);
    void setButtonMode(bool);
    bool getButtonMode();
    void setServerHandle(httpd_handle_t);
    httpd_handle_t getServerHandle();
    void DisconnectPlayer(uint8_t);
    void setGameData(uint8_t);
    uint8_t getGameData();
};
void removePlayerC(void *);

struct sess_cxt{
    Lobby * lobby;
    Player * player;
};

struct LobbyandLED{
    Lobby * lobby;
    DLEDController * LEDcontroller;
};