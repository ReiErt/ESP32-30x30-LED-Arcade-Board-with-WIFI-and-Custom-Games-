// Author Lukas Landgraf
#include "lobby.h"


Lobby::Lobby(){
    vSemaphoreCreateBinary(GameIDS);
    vSemaphoreCreateBinary(PlayersS);
    vSemaphoreCreateBinary(StateS);
    vSemaphoreCreateBinary(ButtonModeS);
    vSemaphoreCreateBinary(ServerHandleS);
    vSemaphoreCreateBinary(GameDataS);
}
Lobby::~Lobby(){

}

LobbyState Lobby::getState(){
    xSemaphoreTake(StateS, portMAX_DELAY);
    LobbyState l = state;
    xSemaphoreGive(StateS);
    return l;
}
void Lobby::setState(LobbyState set){
    xSemaphoreTake(StateS, portMAX_DELAY);
    state = set;
    xSemaphoreGive(StateS);
}
uint8_t Lobby::getPlayerConected(){
    xSemaphoreTake(PlayersS, portMAX_DELAY);
    uint8_t conected = PlayerConected;
    xSemaphoreGive(PlayersS);
    return conected;
}
void Lobby::setMaxPlayer(uint8_t set){
    xSemaphoreTake(PlayersS, portMAX_DELAY);
    MaxPlayer = set;
    xSemaphoreGive(PlayersS);
}
uint8_t Lobby::getMaxPlayer(){
    xSemaphoreTake(PlayersS, portMAX_DELAY);
    uint8_t max = MaxPlayer;
    xSemaphoreGive(PlayersS);
    return max;
}
void Lobby::setGameID(uint8_t set){
    xSemaphoreTake(GameIDS, portMAX_DELAY);
    GameID = set;
    xSemaphoreGive(GameIDS);
}
uint8_t Lobby::getGameID(){
    xSemaphoreTake(GameIDS, portMAX_DELAY);
    uint8_t id = GameID;
    xSemaphoreGive(GameIDS);
    return id;
}
// Fügt Spieler hinzu falls ein Slot frei ist
// Falls die macimale Spieleranzahl erreicht wird wird Lobbystate zu game_running geändert
bool Lobby::addPlayer(httpd_req_t * req){
    xSemaphoreTake(PlayersS, portMAX_DELAY);
    httpd_ws_frame_t ws_frame;
    uint8_t buff[1];
    ws_frame.payload = (uint8_t *)buff;
    ws_frame.len = strlen((char*)buff);
    ws_frame.type = HTTPD_WS_TYPE_TEXT;
    for(int i = 0; i < MaxPlayer; i++){
        if(!Players[i].getconnected()){
            // wird in removePlayerC deleted
            // der free_ctx wird bei webscoekt schließung aufgerufen 
            req->sess_ctx = new sess_cxt{this, &Players[i]};
            req->free_ctx = removePlayerC;
            Players[i].setconnected(true);
            Players[i].setsockfd(httpd_req_to_sockfd(req));
            PlayerConected++;
            //send PlayerID on register
            buff[0] = i+1;
            httpd_ws_send_frame(req, &ws_frame);
            printf("Player registerd\n"); 
            if(PlayerConected == MaxPlayer){
                setState(LobbyState::game_running);
            }
            xSemaphoreGive(PlayersS);
            return true;
        }
    }
    //send 0 on failed register
    buff[0] = 0;
    httpd_ws_send_frame(req, &ws_frame);
    xSemaphoreGive(PlayersS);
    return true;
}

Player * Lobby::getPlayer(uint8_t ID){
    return &Players[ID];
}
// gibt Spieler bei Websocket Schließung wieder frei
// und schließt Lobby Falls 0 Spieler verbunden sind
void removePlayerC(void * cxt){
    ((sess_cxt *)cxt)->player->setconnected(false);
    ((sess_cxt *)cxt)->lobby->reducePlayercount();
    printf("Player removed\n");
    if(((sess_cxt *)cxt)->lobby->getPlayerConected() == 0){
        ((sess_cxt *)cxt)->lobby->setState(LobbyState::lobby_closed);
        ((sess_cxt *)cxt)->lobby->setButtonMode(true);
        printf("lobby close\n");
    }
    ((sess_cxt *)cxt)->lobby = nullptr;
    ((sess_cxt *)cxt)->player = nullptr;
    delete (sess_cxt *)cxt;
    cxt = nullptr;
}

void Lobby::reducePlayercount(){
    xSemaphoreTake(PlayersS, portMAX_DELAY);
    PlayerConected--;
    xSemaphoreGive(PlayersS);
}
// true = loslassen Befehl wird registriert
// false = loslassen Befehl wird ignoriert. Somit bleibt Knopf solange gedrückt bis er selbständig auf false gesetzt wird
void Lobby::setButtonMode(bool temp){
    xSemaphoreTake(ButtonModeS, portMAX_DELAY);
    ButtonMode = temp;
    xSemaphoreGive(ButtonModeS);
}
bool Lobby::getButtonMode(){
    xSemaphoreTake(ButtonModeS, portMAX_DELAY);
    bool temp = ButtonMode;
    xSemaphoreGive(ButtonModeS);
    return temp;
}

void Lobby::setServerHandle(httpd_handle_t temp){
    xSemaphoreTake(ServerHandleS, portMAX_DELAY);
    ServerHandle = temp;
    xSemaphoreGive(ServerHandleS);
}

httpd_handle_t Lobby::getServerHandle(){
    xSemaphoreTake(ServerHandleS, portMAX_DELAY);
    httpd_handle_t  temp = ServerHandle;
    xSemaphoreGive(ServerHandleS);    
    return temp;
}
// Schließt die Websocekt des Spielers
void Lobby::DisconnectPlayer(uint8_t PlayerId){
    xSemaphoreTake(ServerHandleS, portMAX_DELAY);
    httpd_sess_trigger_close(ServerHandle, Players[PlayerId].getsockfd());
    xSemaphoreGive(ServerHandleS);
}

void Lobby::setGameData(uint8_t temp){
    xSemaphoreTake(GameDataS, portMAX_DELAY);
    GameData = temp;
    xSemaphoreGive(GameDataS);
}

uint8_t Lobby::getGameData(){
    xSemaphoreTake(GameDataS, portMAX_DELAY);
    uint8_t temp = GameData;
    xSemaphoreGive(GameDataS);
    return temp;
}

Player::Player(){
    vSemaphoreCreateBinary(connectedS);
    vSemaphoreCreateBinary(sockfdS);
}
Player::~Player(){

}

bool Player::getconnected(){
    xSemaphoreTake(connectedS, portMAX_DELAY);
    bool ret = connected;
    xSemaphoreGive(connectedS);
    return ret;
}

void Player::setconnected(bool set){
    xSemaphoreTake(connectedS, portMAX_DELAY);
    connected = set;
    xSemaphoreGive(connectedS);
}

int Player::getsockfd(){
    xSemaphoreTake(sockfdS, portMAX_DELAY);
    int temp = sockfd;
    xSemaphoreGive(sockfdS);
    return temp;
}

void Player::setsockfd(int temp){
    xSemaphoreTake(sockfdS, portMAX_DELAY);
    sockfd = temp;
    xSemaphoreGive(sockfdS);   
}

Button::Button(){
    vSemaphoreCreateBinary(ButtonS);
}
Button::~Button(){

}
bool Button::getButton(){
    xSemaphoreTake(ButtonS, portMAX_DELAY);
    bool state = button;
    xSemaphoreGive(ButtonS);
    return state;
}
void Button::setButton(bool set){
    xSemaphoreTake(ButtonS, portMAX_DELAY);
    button = set;
    xSemaphoreGive(ButtonS); 
}