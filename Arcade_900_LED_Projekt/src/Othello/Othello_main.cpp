// Author Lukas Landgraf

#include "Othello_main.h"
#include "Arena.h"
// zeigt ein nues Bild alle 2000 Loops an
// und erlaubt Knopf eignaben alle 2000 Loops
#define frameLimit 2000


void Othello_main(Lobby * lobby, DLEDController * LED){
    printf("start Othello\n");
    Arena arena{lobby->getGameData(), lobby->getMaxPlayer(), LED, lobby};
    uint8_t currentPlayer = 0;
    uint8_t currentX = arena.getWidth() / 2;
    uint8_t currentY = arena.getHeight() / 2;
    Player * p = lobby->getPlayer(currentPlayer);
    arena.PlayerPopUp(currentPlayer, p);
    arena.DisplayMap(currentY, currentX);
    int frame = 0;
    while(true){
        // falls das Spiel beendet ist können keine Aktionen mehr durchgeführt werden
        if(!arena.getGamefinished()){
            frame++;
            if(frame == frameLimit){
                arena.DisplayMap(currentY, currentX);
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }
            // Stein setzen
            if(p->Buttons[A].getButton()){
                p->Buttons[A].setButton(false);
                if(arena.applymove(currentY, currentX, currentPlayer, p)){
                    bool PlayerHasMove = false;
                    // finde nächsten Spieler
                    while(!PlayerHasMove){
                        for(uint8_t i = 1; i <= arena.getMaxPlayer(); i++){
                            if(arena.validMoveAvailable((currentPlayer + i) % arena.getMaxPlayer())){
                                currentPlayer = ((currentPlayer + i) % arena.getMaxPlayer());
                                currentX = arena.getWidth() / 2;
                                currentY = arena.getHeight() / 2;
                                PlayerHasMove = true;
                                p = lobby->getPlayer(currentPlayer);
                                arena.PlayerPopUp(currentPlayer, p);
                                break;
                            }
                        }
                        // Falls keine Spieler setzen und das Spiel in der Bomphase ist 
                        // beende das Spiel
                        if(!PlayerHasMove && arena.getBombphase()){
                            arena.endGame();
                            break;
                        }
                        // Falls keine Spieler setzen kann starte Bombenphase
                        if(!PlayerHasMove && !arena.getBombphase()){
                            arena.setBombphase();
                            currentPlayer = arena.getMaxPlayer() - 1;
                            PlayerHasMove = false;
                        }
                    }
                }
                else{
                    printf("invalid move\n");
                }
            }
            // curser kann nur alle paar Frames bewgt werden um Steuerung zu erleichtern
            if(p->Buttons[Down].getButton() && currentY < arena.getHeight() - 1 && frame == frameLimit){
                currentY++;
            }
            if(p->Buttons[Up].getButton() && currentY > 0 && frame == frameLimit){
                currentY--;
            }
            if(p->Buttons[Left].getButton() && currentX > 0 && frame == frameLimit){
                currentX--;
            }
            if(p->Buttons[Right].getButton() && currentX < arena.getWidth() - 1 && frame == frameLimit){
                currentX++;
            }
            if(frame == frameLimit){
                frame = 0;
            }
        }
        // Abfrage ob Spiel beendet werden soll
        if(lobby->getState() == LobbyState::lobby_closed){
            // Falls spiel bereits beendet wurde keinen neuen Gewinner berechnen
            if(!arena.getGamefinished()){
                arena.endGame();
            }
            LED->ClearLEDBuffer();
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            return;
        }
    }
}



