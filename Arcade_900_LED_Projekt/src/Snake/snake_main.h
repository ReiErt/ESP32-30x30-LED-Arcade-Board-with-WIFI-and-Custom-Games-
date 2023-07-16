#pragma once 
#include "lobby.h"
//----------------------------------------------------------------------------------
//Defines 
#define MAXSNAKESIZE 50
//----------------------------------------------------------------------------------
class LED_point {
    private: 
        int8_t x; 
        int8_t y; 
    public: 
        LED_point(); 
        LED_point(int8_t x, int8_t y);
        void setLED_point(uint8_t x, uint8_t y, uint8_t z);
        int8_t getX(); 
        int8_t getY(); 
        void set_position(int8_t x, int8_t y); //for food-set x,y
        void move_up(); 
        void move_down();
        void move_right(); 
        void move_left(); 
        void copy_body(LED_point *); 
};

class Snake {
    public:  
        LED_point * body[MAXSNAKESIZE]; //array von punkten aus denen die Schlange besteht
        char direction; //richtung der schlange 
        uint8_t size; //größe der schlange
        LED_point food;
        bool alive = true;
        Snake(int8_t, int8_t);
        void append_body(int8_t x, int8_t y);
        bool check_collision();
        void turn_up(); 
        void turn_down();
        void turn_left();
        void turn_right(); 
}; 

void print_walls();
void setend(uint8_t x, uint8_t y, uint8_t z );
void move(Snake * snake1, Snake * snake2);
bool check_game_over(Snake * snake1, Snake * snake2);
void Snake_main(Lobby * lobby);