// Author Lukas Landgraf
#include "Arena.h"

// erstellt Spieler anhand von Spieleranzahl und verteilt die Farben
// speicehrt Lobby und LED in Arena ab
// und ladt die ausgewählte map
Arena::Arena(uint8_t MapID, uint8_t _MaxPlayer, DLEDController * _LED, Lobby * _lobby): LED{_LED}, MaxPlayer{_MaxPlayer},lobby{_lobby}{
    printf("create Arena\n");
    this->Players = new Othello_Player[MaxPlayer];
    for(uint8_t i = 0; i < MaxPlayer; i++){
        this->Players[i].PlayerId = i + '1';
        switch(i){
            case 0:{ // player 1 = green
                Players[i].r = 0;
                Players[i].g = 50; //255
                Players[i].b = 0;
            break;
            }
            case 1:{ // player 2 = blue
                Players[i].r = 0;
                Players[i].g = 0;
                Players[i].b = 50; // 255
                break;
            }
            case 2:{ // player 3 = yellow
                Players[i].r = 50; //255
                Players[i].g = 50; //255
                Players[i].b = 0;
                break;
            }
            case 3:{ // player 4 pink
                Players[i].r = 255;
                Players[i].g = 0;
                Players[i].b = 127;
                break;
            }
        }
    }
    loadMap(MapID);
}

Arena::~Arena(){
    for(uint8_t i = 0; i < this->height; i++){
        delete[] this->Field[i];
    }
    delete[] this->Field;
    delete[] this->Players;
}

// Lädt map aus Datei
void Arena::loadMap(uint8_t MapID){
    printf("loadmap\n");
    FILE * file;
    switch(MapID){
        case 0:{
            file = fopen("/data/Othello/Othello.map", "r");
            break;
        }
        case 1:{
            file = fopen("/data/Othello/2_Spieler.map", "r");
            break;
        }
        case 2:{
            file = fopen("/data/Othello/3_Spieler.map", "r");
            break;
        }
        case 3:{
            file = fopen("/data/Othello/4_Spieler.map", "r");
            break;
        }
        case 4:{
            file = fopen("/data/Othello/test.map", "r");
            break;
        }
        default:{
            file = fopen("/data/Othello/Othello.map", "r");
            break;
        }
    }
    if (!file) {
        printf("Couldn't open file %d\n", MapID);
    }
    else{
        printf("File open\n");
    }
    char Buffer[250];
    fgets(Buffer, 250, file);
    // Lädt und verteilt Überschreibsteine
    for(uint8_t i = 0; i < this->MaxPlayer; i++){
        this->Players[i].overwriteStones = atoi(Buffer);
    }
    printf("read overwriteStones\n");

    fgets(Buffer, 250, file);
    char * c = strchr(Buffer, ' ');
    *c = '\0';
    // Lädt und verteilt Bomben
    for(uint8_t i = 0; i < this->MaxPlayer; i++){
        this->Players[i].Bombs = atoi(Buffer);
    }
    // speicehrt bombenradius
    this->Bombradius = atoi(c + 1);
    printf("read Bombs\n");

    fgets(Buffer, 250, file);
    c = strchr(Buffer, ' ');
    *c = '\0';
    // erstellt map array
    this->height = atoi(Buffer);
    this->width = atoi(c + 1);
    printf("read Map size Height: %d width: %d\n", height, width);
    this->Field = new Position*[this->height];
    for(uint8_t i = 0; i < this->height; i++){
        this->Field[i] = new Position[this->width];
    }
    printf("created Field\n");
    // kopiert Map von Datei in Map array
    for(uint8_t i = 0; i < this->height; i++){
        fgets(Buffer, 250, file);
        char * type = strtok(Buffer," \0");
        for(uint8_t j = 0; j < this->width; j++){
            this->Field[i][j].type = type[0];
            type = strtok(NULL," \0");
        }
    }
    printf("read Map\n");
    // Speichert nachbarn eines Felder in dem Feld ab
    // War ursprünglich für Transitionen gedacht gab aber keinen Grund diese Implementierung zu ändern
    for(int y = 0; y < this->height; y++){
        for(int x = 0; x < this->width; x++){
            Field[y][x].closePositions[0] = validPosition(y - 1, x - 1);
            Field[y][x].closePositions[1] = validPosition(y - 1, x);
            Field[y][x].closePositions[2] = validPosition(y - 1, x + 1);
            Field[y][x].closePositions[3] = validPosition(y, x + 1);
            Field[y][x].closePositions[4] = validPosition(y + 1, x + 1);
            Field[y][x].closePositions[5] = validPosition(y + 1, x);
            Field[y][x].closePositions[6] = validPosition(y + 1, x - 1);
            Field[y][x].closePositions[7] = validPosition(y, x - 1);
        }
    }
    
    //Transitionen würden jetzt aus der Datei ausgelesen werden und das benachbarte Felder array verändern 
    printf("finished loading map\n");
}


// überprüft ob Feld ein besetzbares Feld ist
// bei besetzbare Felder also alles außer Wänden'-' oder auserhalb der Karte
// wird ein pointer auf das Feld zurückgegeben
// ansonsten nullptr
Position * Arena::validPosition(int y, int x){
    if(y < 0 || x < 0 || x > width - 1 || y > height - 1){
        return nullptr;
    }
    else if(Field[y][x].type == '-'){
        return nullptr;
    }
    else{
        return &Field[y][x];
    } 
}
// überprüft für einen Spieler ob ein Zug möglich ist indem jedes Feld auf der Map abgefragt wird
int Arena::validMoveAvailable(uint8_t Player){
    for(uint8_t y = 0; y < height; y++){
        for(uint8_t x = 0; x < width; x++){
            if(validMove(y, x, Player, nullptr)){
                return true;
            }
        }
    }
    return false;
}

// überprüft ob ein Zug auf dieses Feld erlaubt ist 
// Falls conqueredPositions != nullptr && der Zug valide ist 
// werden eingenommene Felder abgespeichert und können anschießend verändert werden
bool Arena::validMove(uint8_t y, uint8_t x, uint8_t Player, std::vector<Position *> * conqueredPositions){
    bool valid = false;
    // 1.Phase
    if(!Bombphase){
        // auf Wände kann nicht gestzt werden
        if(Field[y][x].type == '-'){
            valid = false;
        }
        else if(Players[Player].overwriteStones > 0 || Field[y][x].type == '0' || Field[y][x].type == 'b' || Field[y][x].type == 'i' || Field[y][x].type == 'c'){
            Position * lastpos = &Field[y][x];
            // überprüft in alle acht Richtungen ob der Zug valide ist
            for(uint8_t i = 0; i < 8; i++){
                Position * pos = Field[y][x].closePositions[i];
                // es muss mindestens ein Feld eingenommen werden
                if(pos == nullptr || pos->type == Players[Player].PlayerId || pos->type == '-' || pos->type == 'b' || pos->type == 'i' || pos->type == 'c' || pos->type == '0'){
                    continue;
                }
                else{
                    if(pos->validmoveRecursive(lastpos, Players[Player].PlayerId, conqueredPositions)){
                        valid = true;
                    }
                }
            }
            // Auf Expansionsfelder 'x' können mit Überschreibsteinen auch ohne validen Zuge eingenommen werden
            if(Field[y][x].type == 'x' && Players[Player].overwriteStones > 0){
                valid = true;
            }
            if(conqueredPositions != nullptr && valid){
                conqueredPositions->push_back(lastpos);
            }
        }
    }
    //2.Phase
    else{
        // auf Wände können keine Bomben gesetzt werden
        if(Field[y][x].type == '-'){
            valid = false;
        } 
        else if(Players[Player].Bombs > 0 && Field[y][x].type != '-'){
            valid = true;
            Position * lastpos = &Field[y][x];
            for(uint8_t i = 0; i < 8; i++){
                Position * pos = Field[y][x].closePositions[i];
                if(pos == nullptr || pos->type == '-'){
                    continue;
                }
                else if(Bombradius > 0){
                    pos->Bombrecursive(conqueredPositions, Bombradius);
                }
            }
            if(conqueredPositions != nullptr){
                    conqueredPositions->push_back(lastpos);
            }
        }
    }
    return valid;
}
// führt ausgewählten Zug aus
bool Arena::applymove(uint8_t y, uint8_t x, uint8_t PlayerIndex, Player * p){
    std::vector<Position *> conqueredPositions;
    // hole alle eingenommen Zug mit conqueredPositions
    // falls nicht erlaubter Zug gehe aus der Funktion
    if(validMove(y, x, PlayerIndex, &conqueredPositions)){
        // 1. Phase
        if(!Bombphase){
            //speicher Typ des Feldes auf das gesetzt wurde für die sonder Felder
            uint8_t type = Field[y][x].type;
            // ändere Typ der Felder zu neuem Besitzer
            for(Position * pos : conqueredPositions){
                pos->type = Players[PlayerIndex].PlayerId;
            }
            switch(type){
                case 'c':{ //Wahlfeld
                    uint8_t borderx = 0, bordery = 0;
                    // Schleife für Auswahl
                    while(true){
                        LED->ClearLEDBuffer();
                        // Bildschirm Rahmen in Farbe des Spielers der gerade dran ist
                        for(uint8_t i = 0; i < 30; i++){
                            LED->setDataArray(i, 0, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(0, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(i, 29, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(29, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }
                        // zeige Wählbare Spieler an
                        LED->SymbolOnLED(7, 12, Players[0].r, Players[0].g, Players[0].b, 'P');
                        LED->SymbolOnLED(11, 12, Players[0].r, Players[0].g, Players[0].b, '1');

                        LED->SymbolOnLED(16, 12, Players[1].r, Players[1].g, Players[1].b, 'P');
                        LED->SymbolOnLED(20, 12, Players[1].r, Players[1].g, Players[1].b, '2');
                        if(MaxPlayer > 2){
                            LED->SymbolOnLED(7, 21, Players[2].r, Players[2].g, Players[2].b, 'P');
                            LED->SymbolOnLED(11, 21, Players[2].r, Players[2].g, Players[2].b, '3');
                        }
                        if(MaxPlayer > 3){
                            LED->SymbolOnLED(16, 21, Players[3].r, Players[3].g, Players[3].b, 'P');
                            LED->SymbolOnLED(20, 21, Players[3].r, Players[3].g, Players[3].b, '4');
                        }

                        uint8_t cordx = 7;
                        uint8_t cordy = 12;
                        
                        if(borderx == 1){
                            cordx = 16;
                        }
                        if(bordery == 1){
                            cordy = 21;
                        }
                        //Rahmen um gerade ausgewählten Spieler
                        for(uint8_t i = 0; i < 9; i++){
                            LED->setDataArray(cordx - 2, cordy + 2 - i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(cordx + 8, cordy + 2 - i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }
                        for(uint8_t i = 0; i < 11; i++){
                            LED->setDataArray(cordx - 2 + i,  cordy - 6, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(cordx - 2 + i,  cordy + 2, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }

                        LED->SetLEDs();
                        vTaskDelay(20 / portTICK_PERIOD_MS);
                        // ändere ausgwählten Spieler              
                        if(p->Buttons[Left].getButton() || p->Buttons[Right].getButton()){
                            p->Buttons[Left].setButton(false);
                            p->Buttons[Right].setButton(false);
                            if(((borderx + 1) % 2 + bordery * 2) < MaxPlayer){
                                borderx = (borderx + 1) % 2;
                            }
                        }
                        if(p->Buttons[Down].getButton() || p->Buttons[Up].getButton()){
                            p->Buttons[Down].setButton(false);
                            p->Buttons[Up].setButton(false);
                            if((borderx + ((bordery + 1) % 2) * 2) < MaxPlayer){
                                bordery = (bordery + 1) % 2;
                            }
                        }
                        if(p->Buttons[A].getButton()){
                            //tausche Steine mit ausgewählten Spieler
                            p->Buttons[A].setButton(false);
                            uint8_t i = borderx + bordery * 2;
                            std::vector<Position *> temp, temp2;
                            // finde alle Felder von beiden Spielern
                            for(uint8_t k = 0; k < height; k++){
                                for(uint8_t j = 0; j < width; j++){
                                    if(Field[k][j].type == Players[i].PlayerId)
                                        temp.push_back(&Field[k][j]);
                                }
                            }
                            for(uint8_t k = 0; k < height; k++){
                                for(uint8_t j = 0; j < width; j++){
                                    if(Field[k][j].type == Players[PlayerIndex].PlayerId)
                                        temp2.push_back(&Field[k][j]);
                                }
                            }
                            // tausche
                            for(Position * pos : temp){
                                pos->type = Players[PlayerIndex].PlayerId;
                            }
                            for(Position * pos : temp2){
                                pos->type = Players[i].PlayerId;
                            }
                            break;
                        }
                        // Abfrage ob Spiel beendet werden soll
                        if(lobby->getState() == LobbyState::lobby_closed){
                            endGame();
                            break;
                        }
                    }
                    break;
                }
                case 'i':{ //inversion Feld
                    //tauscht alle steine der Spieler
                    std::vector<Position *> temp;
                    for(uint8_t k = 0; k < height; k++){
                        for(uint8_t j = 0; j < width; j++){
                            if(Field[k][j].type == Players[0].PlayerId){
                                temp.push_back(&Field[k][j]);
                            }
                        }
                    }
                    for(uint8_t i = 0; i < MaxPlayer; i++){
                        std::vector<Position *> temp2;
                        for(uint8_t k = 0; k < height; k++){
                            for(uint8_t j = 0; j < width; j++){
                                if(Field[k][j].type == Players[(i + 1) % MaxPlayer].PlayerId){
                                    temp2.push_back(&Field[k][j]);
                                }
                            }
                        }
                        for(Position * pos : temp){
                            pos->type = Players[(i + 1) % MaxPlayer].PlayerId;
                        }
                        temp = temp2;
                    }
                    break;
                }
                case 'b':{//bonus Feld
                    uint8_t Bomb = 0;
                    while(true){
                        LED->ClearLEDBuffer();
                        //Rahmen um Bildschirm in Spielerfarbe
                        for(uint8_t i = 0; i < 30; i++){
                            LED->setDataArray(i, 0, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(0, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(i, 29, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(29, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }  
                        // Symbole für Überschreibstein und Bomben
                        uint8_t xcords[] = {9, 17};
                        uint8_t ycords[] = {17, 17};
                        LED->SymbolOnLED(xcords[0], ycords[0], Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, 'O');

                        LED->SymbolOnLED(xcords[1], ycords[1], Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, 'B');
                        
                        //Rahmen um ausgwähltes Objekt
                        for(uint8_t i = 0; i < 9; i++){
                            LED->setDataArray(xcords[Bomb] - 2, ycords[Bomb] + 2 - i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(xcords[Bomb] + 4, ycords[Bomb] + 2 - i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }
                        for(uint8_t i = 0; i < 7; i++){
                            LED->setDataArray(xcords[Bomb] - 2 + i, ycords[Bomb] + 2, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                            LED->setDataArray(xcords[Bomb] - 2 + i, ycords[Bomb] - 6, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
                        }
                        LED->SetLEDs();
                        vTaskDelay(20 / portTICK_PERIOD_MS);
                        //Steuerung  
                        if(p->Buttons[Left].getButton() || p->Buttons[Right].getButton()){
                            p->Buttons[Left].setButton(false);
                            p->Buttons[Right].setButton(false);
                            Bomb = (Bomb + 1) % 2;
                        }
                        if(p->Buttons[A].getButton()){
                            p->Buttons[A].setButton(false);
                            if(Bomb == 1){
                                Players[PlayerIndex].Bombs++;
                            }
                            else{
                                Players[PlayerIndex].overwriteStones++;
                            }
                            break;
                        }
                        // Abfrage ob Spiel beendet werden soll
                        if(lobby->getState() == LobbyState::lobby_closed){
                            endGame();
                            break;
                        }
                    }
                    break;
                }
                // Falls auf einen bereits bestztes Feld gestzt wird muss ein Überschreibstein entfernt werden
                case '1':
                case '2':
                case '3':
                case '4':
                case 'x':
                    Players[PlayerIndex].overwriteStones--;
            }
        }
        // 2.Phase
        // in der zweiten Phase können sachen nur gesprengt werden
        else{
            for(Position * pos : conqueredPositions){
                pos->type = '-';
            }
            Players[PlayerIndex].Bombs--;
        }
        return true;
    }
    //falls zug invalide
    else{
        return false;
    }
}

// Anzeige welcher Spieler als nächstes dran ist und wartet bis dieser ein en Knopf drückt
void Arena::PlayerPopUp(uint8_t PlayerIndex, Player * p){
    // Text ausgabe um ohne Board zu Debbugen
    /*
        std::vector<Position *> temp;
        for(uint8_t k = 0; k < height; k++){
            for(uint8_t j = 0; j < width; j++){
                if(Field[k][j].type == Players[PlayerIndex].PlayerId)
                    temp.push_back(&Field[k][j]);
            }
        }
        printf("Player %d O: %d B: %d  %d\n", PlayerIndex + 1, Players[PlayerIndex].overwriteStones, Players[PlayerIndex].Bombs, temp.size());
    */

    LED->ClearLEDBuffer();
    // Bild ausgabe für Spieler, Überschreibsteine und Bomben in Farbe von Spieler
    LED->SymbolOnLED(12, 11, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, 'P');
    LED->SymbolOnLED(16, 11, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, Players[PlayerIndex].PlayerId);

    LED->SymbolOnLED(10, 18, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, 'O');
    LED->SymbolOnLED(14, 18, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, ':');
    if( Players[PlayerIndex].overwriteStones / 10 != 0){
        LED->SymbolOnLED(18, 18, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].overwriteStones / 10) + '0');
        LED->SymbolOnLED(22, 18, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].overwriteStones % 10) + '0');
    }
    LED->SymbolOnLED(18, 18, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].overwriteStones % 10) + '0');
    

    LED->SymbolOnLED(10, 25, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, 'B');
    LED->SymbolOnLED(14, 25, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, ':');
    if(Players[PlayerIndex].Bombs / 10 != 0){
        LED->SymbolOnLED(18, 25, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].Bombs / 10) + '0');
        LED->SymbolOnLED(22, 25, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].Bombs % 10) + '0');
    }
    LED->SymbolOnLED(18, 25, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b, (Players[PlayerIndex].Bombs % 10) + '0');

    // Rahmen in Farbe von Spieler
    for(uint8_t i = 0; i < 30; i++){
        LED->setDataArray(i, 0, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
        LED->setDataArray(0, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
        LED->setDataArray(i, 29, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
        LED->setDataArray(29, i, Players[PlayerIndex].r, Players[PlayerIndex].g, Players[PlayerIndex].b);
    }
    LED->SetLEDs();
    // warten auf eingabe von Spieler
    while(true){
        vTaskDelay(20 / portTICK_PERIOD_MS);
        for(uint8_t i = 0; i < 5; i++){
            if(p->Buttons[i].getButton()){
                p->Buttons[i].setButton(false);
                return;
            }
        }
        // Abfrage ob Spiel beendet werden soll
        if(lobby->getState() == LobbyState::lobby_closed){
            endGame();
            return;
        }
    }
}

// erstellt neues Bild
void Arena::DisplayMap(uint8_t currenty, uint8_t currentx){
    // Text ausgabe um ohne Board zu Debbugen
    /*  char Display[30][30];
        for(int y = -14; y <= 15; y++){
            for(int x = -14; x <= 15; x++){
                if(currenty + y < 0 || currenty + y >= height || currentx + x < 0 || currentx + x >= width){
                    Display[y + 14][x + 14] = '-'; 
                }
                else{
                    Display[y + 14][x + 14] = Field[currenty + y][currentx + x].type; 
                }
                if(y == 0 && x == 0){
                    Display[y + 14][x + 14] = '*';
                }
                printf("%c ", Display[y + 14][x + 14]);
            }
            printf("\n");
        }
        printf("y: %d, x: %d\n", currenty, currentx);*/
    // Ausgabe aller Felder um den Cursor herum
        for(int y = -14; y <= 15; y++){
            for(int x = -14; x <= 15; x++){
                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0;
                if(currenty + y < 0 || currenty + y >= height || currentx + x < 0 || currentx + x >= width){
                    //Auserhalb der Karte = Wand(rot)
                    r = 25;
                    g = 0;
                    b = 0;
                }
                else{
                    switch(Field[currenty + y][currentx + x].type){
                        case '0':{ // Leeres Feld = keine Farbe
                            r = 0;
                            g = 0;
                            b = 0;
                            break;
                        }
                        case '1':{ // Spieler 1 = grün
                            r = Players[0].r;
                            g = Players[0].g;
                            b = Players[0].b;
                            break;
                        }
                        case '2':{ // Spieler 2 = blau
                            r = Players[1].r;
                            g = Players[1].g;
                            b = Players[1].b;
                            break;
                        }
                        case '3':{ // Spieler 3 = gelb
                            r = Players[2].r;
                            g = Players[2].g;
                            b = Players[2].b;
                            break;
                        }
                        case '4':{ // Spieler 4 pink
                            r = Players[3].r;
                            g = Players[3].g;
                            b = Players[3].b;
                            break;
                        }
                        case '-':{ // Wand = rot
                            r = 25;
                            g = 0;
                            b = 0;
                            break;
                        }
                        case 'c':{ // Wahlfeld = orange
                            r = 255;
                            g = 128;
                            b = 0;
                            break;
                        }
                        case 'i':{ // inversionsfeld wechselt zwischen allen Farben der Spieler
                            r = Players[inv].r;
                            g = Players[inv].g;
                            b = Players[inv].b;
                            break;
                        }
                        case 'b':{ // Bonusfeld
                            r = 0;
                            g = 255;
                            b = 170;
                            break;
                        }
                        case 'x':{ // Expansionsfeld
                            r = 40;
                            g = 40;
                            b = 40;
                            break;
                        }
                    }
                }
                if(y == 0 && x == 0){ // Cursor wechselt jeden Frame zwischen weiß und dem Typen des Feldes
                    if(lastCursorWhite){
                        lastCursorWhite = false;
                    }
                    else{
                        lastCursorWhite = true;
                        r = 255;
                        g = 255;
                        b = 255;
                    }
                }
                LED->setDataArray(x + 14, y + 14, r, g, b);
            }
        }
        inv = (inv + 1) % MaxPlayer;
        LED->SetLEDs();
        vTaskDelay(20 / portTICK_PERIOD_MS);
}

// berechent Gewinner und zeigt ihn an
// ändert Status des Spiels zu Beendet 
// und disconnected alle Spieler
void Arena::endGame(){
    std::vector<Position *> temp;
    uint8_t winner = 0;
    for(uint8_t i = 0; i < MaxPlayer; i++){
        std::vector<Position *> temp2;
        for(uint8_t k = 0; k < height; k++){
            for(uint8_t j = 0; j < width; j++){
                if(Field[k][j].type == Players[i].PlayerId)
                    temp2.push_back(&Field[k][j]);
            }
        }
        if(temp.size() < temp2.size()){
            winner = i;
            temp = temp2;
        }
    }

    LED->ClearLEDBuffer();
    for(uint8_t i = 0; i < 30; i++){
        LED->setDataArray(i, 0, Players[winner].r, Players[winner].g, Players[winner].b);
        LED->setDataArray(0, i, Players[winner].r, Players[winner].g, Players[winner].b);
        LED->setDataArray(i, 29, Players[winner].r, Players[winner].g, Players[winner].b);
        LED->setDataArray(29, i, Players[winner].r, Players[winner].g, Players[winner].b);
    } 
    LED->SymbolOnLED(12, 18, Players[winner].r, Players[winner].g, Players[winner].b, 'P');
    LED->SymbolOnLED(16, 18, Players[winner].r, Players[winner].g, Players[winner].b, '1' + winner);
    LED->SetLEDs();
    printf("Winner: %d\n", winner);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    Gamefinished = true;
    for(uint8_t i = 0; i < MaxPlayer; i++){
        lobby->DisconnectPlayer(i);
    }
}
// starte Bombenphase
void Arena::setBombphase(){
    Bombphase = true;
    LED->ClearLEDBuffer();
    for(uint8_t i = 0; i < 30; i++){
        LED->setDataArray(i, 0, 255, 0, 0);
        LED->setDataArray(0, i, 255, 0, 0);
        LED->setDataArray(i, 29, 255, 0, 0);
        LED->setDataArray(29, i, 255, 0, 0);
    }
    LED->SymbolOnLED(4, 18, 255, 0, 0, '2');
    LED->SymbolOnLED(8, 18, 255, 0, 0, 'P');
    LED->SymbolOnLED(12, 18, 255, 0, 0, 'H');
    LED->SymbolOnLED(16, 18, 255, 0, 0, 'A');
    LED->SymbolOnLED(20, 18, 255, 0, 0, 'S');
    LED->SymbolOnLED(24, 18, 255, 0, 0, 'E');
    LED->SetLEDs();
    printf("Bombphase\n");
    while(true){
        vTaskDelay(20 / portTICK_PERIOD_MS);
        for(uint8_t i = 0; i < MaxPlayer; i++){
            Player * p = lobby->getPlayer(i);
            for(uint8_t i = 0; i < 5; i++){
                if(p->Buttons[i].getButton()){
                    p->Buttons[i].setButton(false);
                    return;
                }
            }
        }
        // Abfrage ob Spiel beendet werden soll
        if(lobby->getState() == LobbyState::lobby_closed){
            endGame();
            return;
        }
    }
}