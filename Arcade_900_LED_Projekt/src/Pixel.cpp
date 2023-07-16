#include <iostream>
#include <vector>
#include "Pixel.h"
#include "DStripData.h"
#define matrix_width 30
#define matrix_height 30
#define red_byte 0
#define green_byte 1
#define blue_byte 2



/**
 * @brief create a pixel and put it on the vector
 */
Pixel::Pixel(std::vector<Pixel *>& vector, Pixel::Color c, uint8_t _x_location, uint8_t _y_location)
    {
    pixel_data = new uint8_t[3];
    initialised = false;
    x_location = _x_location;
    y_location = _y_location;
    setColor(c);
    vector.push_back(this);
}

/**
 * @brief 
 * 
 * @param vec 
 * @param item 
 */
void Pixel::removeItem(std::vector<Pixel *>& vec, Pixel * item) 
{
    vec.erase(std::remove(vec.begin(), vec.end(), item), vec.end());
}


/**
* @brief pass pixel color and location to 3D array
*/
void Pixel::moveto3DArray(DStripData& objectA)
{   
    *(*(*(objectA.data_array3D+x_location)+y_location)+0)  = pixel_data[0];
    *(*(*(objectA.data_array3D+x_location)+y_location)+1)  = pixel_data[1];
    *(*(*(objectA.data_array3D+x_location)+y_location)+2)  = pixel_data[2];
}  


Pixel::~Pixel()
{
    //Destroy();
    pixel_data[0] = 0;
    pixel_data[1] = 0;
    pixel_data[2] = 0;
    vTaskDelay(500 / portTICK_PERIOD_MS);
    pixel_data = nullptr;
    delete[] pixel_data;
}

/**
 * @brief Checks three LED bytes in 3D array to see if empty
 */
bool Pixel::isLEDempty(uint8_t row, uint8_t column, DStripData d)
{   // if bytes 0,1,2 are empty, we return true
    if  ((*(*(*(d.data_array3D+column)+row)+0) == 0) && 
         (*(*(*(d.data_array3D+column)+row)+1) == 0) &&
         (*(*(*(d.data_array3D+column)+row)+2) == 0)) 
    {
        printf("Yes, led is empty at [%d][%d]", row, column); 
        return true;
    }
    else 
    {
        printf("No, led is NOT empty at [%d][%d]", row, column); 
        return false;
    }
}

/**
 * @brief Move pixel object 1 LED in any direction
 * down = ++1 y
 * up   = --1 y
 * left = --1 x
 * right= ++1 x
 * *pixel_color stays the same. it is just moved
 */ 
void Pixel::SlidePixel(Pixel& p, Direction dir, DStripData d)
{
    if (dir == Direction::UP){
       y_location = (y_location - 1) % matrix_height;
    }

    if (dir == Direction::UPRIGHT){
       y_location = (y_location - 1) % matrix_height;
       x_location = (x_location + 1) % matrix_width;
    }

    if (dir == Direction::UPLEFT){
        y_location = (y_location - 1) % matrix_height;
        x_location = (x_location - 1) % matrix_width;
    }

    if (dir == Direction::DOWN){
        y_location = (y_location + 1) % matrix_height;
    }

    if (dir == Direction::DOWNRIGHT){
        y_location = (y_location + 1) % matrix_height;
        x_location = (x_location + 1) % matrix_width;
    }

    if (dir == Direction::DOWNLEFT){
        y_location = (y_location + 1) % matrix_height;
        x_location = (x_location - 1) % matrix_width;
    }

    if (dir == Direction::LEFT){
        x_location = (x_location - 1) % matrix_width;
    }

    if (dir == Direction::RIGHT){
        x_location = (x_location + 1) % matrix_width;
    }
}



/**
 * @brief set pixel color
 */
void Pixel::setColor(Color c)
{
    if(c == Color::RED){
        redColor = 255;
        greenColor = 0;
        blueColor = 0;
    }

    if(c == Color::YELLOW){
        redColor = 255;
        greenColor = 255;
        blueColor = 0;
    }

    if(c == Color::GREEN){
        redColor = 0;
        greenColor = 255;
        blueColor = 0;
    }

    if(c == Color::TEAL){
        redColor = 0;
        greenColor = 255;
        blueColor = 255;
    }

    if(c == Color::BLUE){
        redColor = 0;
        greenColor = 0;
        blueColor = 255;
    }

    if(c == Color::PURPLE){
        redColor = 255;
        greenColor = 0;
        blueColor = 255;
    }

    if(c == Color::WHITE){
        redColor = 255;
        greenColor = 255;
        blueColor = 255;
    }
     if(c == Color::OFF){
        redColor = 0;
        greenColor = 0;
        blueColor = 0;
    }
    pixel_data[0] = redColor;
    pixel_data[1] = greenColor;
    pixel_data[2] = blueColor;
    colored = true;
}

