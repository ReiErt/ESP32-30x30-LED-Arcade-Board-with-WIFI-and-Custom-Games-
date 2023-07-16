#pragma once
// #ifndef PIXEL_H
// #define PIXEL_H
#include <stddef.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "DStripData.h"
#define red_byte 0
#define green_byte 1
#define blue_byte 2

class Pixel
{
public:

    enum class Color
    {
        RED,
        YELLOW,
        GREEN,
        TEAL,
        BLUE,
        PURPLE,
        WHITE,
        OFF
    };

    enum class Direction : uint8_t
    {
        UP,
        UPRIGHT,
        UPLEFT,
        DOWN,
        DOWNRIGHT,
        DOWNLEFT,
        LEFT,
        RIGHT
    };

    Pixel(std::vector<Pixel *>& vector, Pixel::Color c, uint8_t x_location, uint8_t y_location);
    ~Pixel(void); 

    bool isInitialised(void)         { return initialised;}
    bool isLEDempty(uint8_t row, uint8_t column, DStripData d); 

    uint8_t get_LED_size(void)       { return LED_size; }
    uint8_t get_x_location(void)     { return x_location; }
    uint8_t get_y_location(void)     { return y_location; }
    uint8_t get_RedColor(void)       { return redColor; }
    uint8_t get_GreenColor(void)     { return greenColor; }
    uint8_t get_BlueColor(void)      { return blueColor; }

    void removeItem(std::vector<Pixel *>& vector, Pixel * array);
    //void Destroy();
    void setColor(Color c); 
    void SlidePixel(Pixel& p, Direction dir, DStripData d);
    // bool DetectLEDCollision(uint8_t row, uint8_t column, DStripData d);
    void moveto3DArray(DStripData& objectA);

    // bool Create(uint8_t _blocks_down, uint8_t _blocks_right);
    // bool Create(Color redColor, Color greenColor, Color blueColor);
    //void MovePixel(Pixel& p, uint8_t row, uint8_t column);
    //void SetPixelColor(Color c, Pixel p);
    // void push_back(std::vector<Pixel>& vector, Pixel& p);
    //void erase(std::vector<Pixel>& vec, Pixel& p);
    
    bool block;
    uint8_t LED_size;
    bool initialised;
    bool colored;
    bool visible;
    uint8_t redColor;
    uint8_t greenColor;
    uint8_t blueColor;

    uint8_t x_location;
    uint8_t y_location;

    uint8_t blocks_down;
    uint8_t blocks_right;
    uint8_t* pixel_data;
    uint8_t length;
    //DStripData& obj;

protected:
private: 
};

// #endif

//void addToVector(std::vector<std::array<uint8_t, 3>>& vector, std::array<uint8_t, 3U>* array);
// void push_back(std::vector<std::array<uint8_t, 3U>>& vec, const std::array<uint8_t, 3U>& element);
//Pixel::Pixel(std::vector<Pixel*>& vec, uint8_t _blocks_down, uint8_t _blocks_right);
//Pixel::Pixel(std::vector<std::array<uint8_t, 3>>& vector, Color c, uint8_t x_location, uint8_t y_location);  
//void Pixel::addToVector(std::vector<Pixel*>& vector);
//static void removeItem(std::vector<Pixel>& vec, Pixel p);
// void removeItem(std::vector<std::array<uint8_t, 3>>& vector, const std::array<uint8_t, 3U>& array);


/*
////////////////////////////////////////////////

Array[4][5][3] = Array_1[5][3], Array_2[5][3], Array_3[5][3], Array_4[5][3];


Array_1[5][3]={{0,1,2},{3,4,5},{6,7,8},{9,10,11},{12,13,14}};

Array_2[5][3]={{15,16,17},{18,19,20},{21,22,23},{24,25,26},{27,28,29}};

Array_3[5][3]={{30,31,32},{33,34,35},{36,37,38},{39,40,41},{42,43,44}};

Array_4[5][3]={{45,46,47},{48,49,50},{51,52,53},{54,55,56},{57,58,59}};

Array[4][5][3] = | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 .... | 59;



uint8_t** data = stripData.data();
// I now have the beginning of my array


// to pass this function, I need to know [x][y][3]
SetPixelPWM(uint8_t row, uint8_t column, enum speed, size);
SetPixelPWM(27,17, fast);


SetPixelPWM(uint8_t row, uint8_t column, enum speed)
    {    //how to keep pwm blinking?
    uint8_t** data = stripData.Data();
    uint8_t PWM_blink = 0;
    for(int i = 0; i < 15; ++i){
        for(int j = 0; j < 50; j+5){
            *(*(*(data+row)+column)+0) = PWM_blink;
            *(*(*(data+row)+column)+1) = PWM_blink;
            *(*(*(data+row)+column)+2) = PWM_blink;
            // or
            data[row][column][0] = PWM_blink;
            data[row][column][1] = PWM_blink;
            data[row][column][2] = PWM_blink;
        }
    }
}

//how to pass setpixel:
SetPixel(0,2,255,0,0);
void SetPixel(uint8_t row, uint8_t column, uint8_t r, uint8_t g, uint8_t b){
    uint8_t** data = stripData.Data();
    // here we want to set a pixel
    
    *(*(*(data+row)+column)+red_byte)     = r;
    *(*(*(data+row)+column)+green_byte) = g;
    *(*(*(data+row)+column)+blue_byte)     = b;
}

//is pixel set?
isPixelSet(3,27);
bool ispixelset(uint8_t row, uint8_t column){
    // and each byte against 0b11111111. If statement is true, then a bit is set. So, return false.
    if (*(*(*(data+row)+column)+red_byte)   & 0xff) { return false;} else {return true;}
    if (*(*(*(data+row)+column)+green_byte) & 0xff) { return false;} else {return true;}
    if (*(*(*(data+row)+column)+blue_byte)  & 0xff) { return false;} else {return true;}
}

//move pixel




MovePixel(4,5,5,5);
void MovePixel(oldRow, oldColumn, newRow, newColumn){
    uint8_t** data = stripData.Data();
    
    *(*(*(data+newRow)+newColumn)+red_byte)        = *(*(*(data+oldRow)+oldColumn)+red_byte);
    *(*(*(data+newRow)+newColumn)+green_byte)    = *(*(*(data+oldRow)+oldColumn)+green_byte);
    *(*(*(data+newRow)+newColumn)+blue_byte)    = *(*(*(data+oldRow)+oldColumn)+blue_byte);
    
    *(*(*(data+oldRow)+oldColumn)+red_byte)   = 0;
}   *(*(*(data+oldRow)+oldColumn)+green_byte) = 0;
    *(*(*(data+oldRow)+oldColumn)+blue_byte)  = 0;
}



////////////////////// This is important for clarification
arry[out][in][inner]
arry[27][17][0]
*(*(arry_begin+out)+17)+0)

*(*(*(arry+out)+in)+inner)


M[i][j][k] ==>  *(*(*(M+i)+j)+k)
*/


// ************** 3D array to RMT MAPPING FUNCTION - begin *****************

// heres how we feed the RMTChannelBuffer with our 3D array:
// read from last row of 3D array starting at the last column.

// row = last; column = last; 
// count backwards until row = last; column = 0; 
// move up one row; row++;

// row = last-1; column = last;
// count backwards until row = last-1; column = 0;
// move up one row; row++;

// row = last-2; column = last
// count backwards until row = last-2; column = 0;
// move up one row; row++;

// row = last-2; column = last
// count backwards until row = last-2; column = 0;
// move up one row; row++;

//until
// row = 0; column = last
// count backwards until row = 0; column = 0;

// ************** 3D array to RMT MAPPING FUNCTION - end *****************