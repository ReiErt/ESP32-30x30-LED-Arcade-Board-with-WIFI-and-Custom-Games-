//--------------------------------------------------------
//Snake Main Funktion, zwei Spieler
//--------------------------------------------------------

#include "snake_main.h"

#define MAXPLAYER 2
#define ERROR 9
#define SNAKE1 0
#define SNAKE2 1
#define FOOD1 2
#define FOOD2 3
#define DELETE 4
#define FOOD_RANGE 28 
#define UPPER_BOARDLIMIT 30
//---------------------------------------------------------

//Variable zum Ansprechen der LEDs
extern DLEDController LEDcontroller;

//lichter für die Ränder, sind nur eine höfliche information den rand nicht zu treffen. Außerdem kann sich auch Futter auf der Wand befinden, wie spinnen zb :)
void print_walls(){
int i = 0, h=29;
for (int j = 0; j < 30; j++){
    for (int k = 0; k < 3; k++){
        LEDcontroller.stripData.data_array3D[i][j][k] = 255;
        LEDcontroller.stripData.data_array3D[h][j][k] = 255;
        LEDcontroller.stripData.data_array3D[j][i][k] = 255;
        LEDcontroller.stripData.data_array3D[j][h][k] = 255;
        }
    }
}
//malt das endzeichen in die linke obere Ecke in Rot
void setend(uint8_t x, uint8_t y, uint8_t z){
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 255;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 0;
        LEDcontroller.SetLEDs();
        vTaskDelay(20 / portTICK_PERIOD_MS);
}
//gibt die LEDpunkte für beide Schlange und ihre Futterelemente aus
void LED_point::setLED_point(uint8_t x, uint8_t y, uint8_t z){
    if(z==SNAKE1){
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 100;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 255;
    }
    else if(z==SNAKE2){
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 100;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 255;
    }
    else if(z==FOOD1) {
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 100;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 255;
    }
    else if(z==FOOD2){
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 100;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 255;
    }
    else if(z==DELETE) {
        LEDcontroller.stripData.data_array3D[x][y][red_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][blue_byte] = 0;
        LEDcontroller.stripData.data_array3D[x][y][green_byte] = 0;
    }
}
LED_point::LED_point(int8_t x, int8_t y){
    this->x = x; 
    this->y = y; 
}
LED_point::LED_point(){ //standard konstruktor
    x=y=10;
}
int8_t LED_point::getX(){
    return x; 
}
int8_t LED_point::getY(){
    return y;
}
void LED_point::move_up(){
    x--;
}
void LED_point::move_down(){
    x++;
}
void LED_point::move_left(){
    y--;
}
void LED_point::move_right(){
    y++;
}
//funktion für das nachfolgen der einzelnen Glieder
void LED_point::copy_body(LED_point * p){
    x = p->x; 
    y = p->y; 
}
//konstruktor der Snake
Snake::Snake(int8_t x, int8_t y){
    size = 1;
    body[0] = new LED_point(x,y); 
    for (int i=1; i<MAXSNAKESIZE; i++){
        body[i]=NULL;   //alle elemente NULL
    }
}
//allgemeiner Setter
void LED_point::set_position(int8_t x, int8_t y){
    this->x = x; 
    this->y = y; 
}
//verlängern der Schlange um ein Glied
void Snake::append_body( int8_t x, int8_t y){
    body[size++] = new LED_point(x,y);
}
void Snake::turn_up(){
    if(direction == 's')//die Zusätzliche Abfrage verhindert dass die schlange in 
        return; //die gegengleiche richtung laufen kann
    else
        direction = 'w'; 
}
void Snake::turn_down(){
    if (direction == 'w' )
        return; 
    else 
        direction = 's';
}
void Snake::turn_left(){
    if (direction == 'd')
        return; 
    else 
        direction = 'a';
}
void Snake::turn_right(){
    if (direction == 'a')
        return; 
    else
        direction = 'd';
}
//Bewegung der Schlangen
void move(Snake * snake1, Snake * snake2){
    
    //löscht letzten snake körper vom LED-board
    snake1->body[snake1->size-1]->setLED_point(snake1->body[snake1->size-1]->getX(), snake1->body[snake1->size-1]->getY(), DELETE);
    snake2->body[snake2->size-1]->setLED_point(snake2->body[snake2->size-1]->getX(), snake2->body[snake2->size-1]->getY(), DELETE);
   
   for(int i=snake1->size-1; i>0; i--){    //folge dem vorherigen Glied 
	 	snake1->body[i]->copy_body(snake1->body[i-1]);
	}
    for(int i=snake2->size-1; i>0; i--){   
	 	snake2->body[i]->copy_body(snake2->body[i-1]);
	}
    //Richtungseinstellung
    switch(snake1->direction){
        case 'w': 
            snake1->body[0]->move_up(); 
            break;
        case 's': 
            snake1->body[0]->move_down(); 
            break; 
        case 'a': 
            snake1->body[0]->move_left();
            break; 
        case 'd': 
            snake1->body[0]->move_right(); 
            break;  
    }
    switch(snake2->direction){
        case 'w': 
            snake2->body[0]->move_up(); 
            break;
        case 's': 
           snake2->body[0]->move_down(); 
            break; 
        case 'a': 
            snake2->body[0]->move_left();
            break; 
        case 'd': 
            snake2->body[0]->move_right(); 
            break;  
    }

    //Kollision mit sich selbst
    if(snake1->body[0]->getX()==UPPER_BOARDLIMIT || snake1->body[0]->getX()<0 || snake1->body[0]->getY()==UPPER_BOARDLIMIT || snake1->body[0]->getY()<0){
        snake1->alive=false; 
        return;
    }
    if(snake2->body[0]->getX()==UPPER_BOARDLIMIT || snake2->body[0]->getX()<0 || snake2->body[0]->getY()==UPPER_BOARDLIMIT || snake2->body[0]->getY()<0){
        snake2->alive=false; 
        return;
    }
    //Schlangenkopf trifft auf Futterobjekt
    if (snake1->food.getX() == snake1->body[0]->getX() && snake1->food.getY()== snake1->body[0]->getY()){
        snake1->food.setLED_point(snake1->food.getX(), snake1->food.getY(), DELETE);
        snake1->append_body(0,0);  
        snake1->food.set_position(rand()%(FOOD_RANGE+1), rand()%(FOOD_RANGE+1));
        snake1->food.setLED_point(snake1->food.getX(), snake1->food.getY(), FOOD1);
    }
    if (snake2->food.getX() == snake2->body[0]->getX() && snake2->food.getY()== snake2->body[0]->getY()){
        snake2->food.setLED_point(snake2->food.getX(), snake2->food.getY(), DELETE);
        snake2->append_body(0,0);  
        snake2->food.set_position(rand()%(FOOD_RANGE+1), rand()%(FOOD_RANGE+1));
        snake2->food.setLED_point(snake2->food.getX(), snake2->food.getY(), FOOD2);
    }
    //Elementweises Printen der Schlangenelemente
    for ( int i =0; i< snake1->size; i++){
        int8_t x= snake1->body[i]->getX();
        int8_t y= snake1->body[i]->getY();
        snake1->body[i]->setLED_point(x,y,SNAKE1);
    }
    for ( int i =0; i< snake2->size; i++){
        int8_t x= snake2->body[i]->getX();
        int8_t y= snake2->body[i]->getY();
        snake2->body[i]->setLED_point(x,y,SNAKE2);
    }
    
    vTaskDelay(200/ portTICK_PERIOD_MS);
}

bool Snake::check_collision() {//Kollisionskontrolle mit sich selbst
    LED_point *head = body[0];
    for (uint8_t i = 2; i < size-1; i++) {
        if (head->getX() == body[i]->getX() && head->getY() == body[i]->getY())
            return true;
    }
    return false;
}

bool check_game_over(Snake * snake1, Snake * snake2){
    bool collision1 = snake1->check_collision();
    bool collision2 = snake2->check_collision();
    LED_point *head1 = snake1->body[0];
    LED_point *head2 = snake2->body[0];
    for (uint8_t i = 0; i < snake2->size; i++) {
        if (head1->getX() == snake2->body[i]->getX() && head1->getY() == snake2->body[i]->getY()) 
        //schlange 1 berührt schlange 2   
        return true; 
    }
    for (uint8_t i = 0; i < snake1->size; i++) {
        if (head2->getX() == snake1->body[i]->getX() && head2->getY() == snake1->body[i]->getY() )
        //schlange 2 beführt schlange 1
        return true; 
    }
    //mit sich selbst
    if(collision1 == true || collision2 == true)
        return true;

    return false; 
}
//MAIN
void Snake_main (Lobby * lobby){
    printf("Starting Snake\n");
    srand( (unsigned) time(NULL));
    Player * p1 = lobby->getPlayer(0);
    Player * p2 = lobby->getPlayer(1);
    Snake snake1(10, 15);
    Snake snake2(20, 15);
    print_walls();
    snake1.alive=true;
    snake2.alive=true;
    bool end = false; 
    snake1.food.set_position(rand()%(FOOD_RANGE+1), rand()%(FOOD_RANGE+1));
    snake1.food.setLED_point(snake1.food.getX(), snake1.food.getY(), FOOD1); 
    snake2.food.set_position(rand()%(FOOD_RANGE+1), rand()%(FOOD_RANGE+1));
    snake2.food.setLED_point(snake2.food.getX(), snake2.food.getY(), FOOD2); 
    while(true){
        end = check_game_over(&snake1, &snake2);
        if(end == true || snake1.alive == false || snake2.alive == false){
            setend(2,0,ERROR);
            setend(3,0,ERROR);
            setend(3,1,ERROR);
            setend(3,2,ERROR);
            setend(3,3,ERROR);
            setend(3,2,ERROR);
            
            for(uint8_t i = 0; i < MAXPLAYER; i++){
                lobby->DisconnectPlayer(i);
                //wenn alle Spieler disconnected sind, wird die lobby geschlossen 
            } 
        }
        if(p1->Buttons[Up].getButton())
        {
            snake1.turn_up();
        }
        if(p2->Buttons[Up].getButton())
        {
            snake2.turn_up();
        }
         if(p1->Buttons[Down].getButton())
        {
            snake1.turn_down();
        }
        if(p2->Buttons[Down].getButton())
        {
            snake2.turn_down();
        }
        if(p1->Buttons[Left].getButton())
        {
            snake1.turn_left();
        }
        if(p2->Buttons[Left].getButton())
        {
            snake2.turn_left();
        }
        if(p1->Buttons[Right].getButton())
        {
            snake1.turn_right();
        }
        if(p2->Buttons[Right].getButton())
        {
            snake2.turn_right();
        }
        move(&snake1, &snake2);
        LEDcontroller.SetLEDs();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if(lobby->getState() == LobbyState::lobby_closed){
            // all players have disconected 
            LEDcontroller.ClearLEDBuffer();
            LEDcontroller.SetLEDs();
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            return;
        }
    
}