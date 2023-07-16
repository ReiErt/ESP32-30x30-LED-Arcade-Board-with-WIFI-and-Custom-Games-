#include "DStripData.h"
#include <new>
#define matrix_width 30
#define matrix_height 30
#define red_byte 0
#define green_byte 1
#define blue_byte 2
// this .cpp holds a 3D array, like this:
//M[i][j][k] ==>  *(*(*(M+i)+j)+k)    
// 3D array!!! creates arr[30][30][3] arr[row][column][led_color]


/**
 * @brief constructor LED Matrix
 */
DStripData::DStripData(void)
{
    data_array3D = nullptr;
    dataArrayLength = 0;
}

DStripData::~DStripData()
{
    Destroy();
}

/**
 * @brief Move create 3D array to save the three byte led information
 */
bool DStripData::Create(uint16_t numberOfLEDs)
{
    Destroy();
    if (numberOfLEDs < 1) return false;

    numLEDs = numberOfLEDs;
    dataArrayLength = bytesPerLED * 900;

    // creation of 3D array 
    //M[i][j][k] ==>  *(*(*(M+i)+j)+k)    
    // 3D array!!! creates arr[30][30][3] arr[row][column][led_color]
    data_array3D = new uint8_t**[matrix_height];
    for (int i = 0; i < matrix_height; i++) // Changed originaly 3
    {
        data_array3D[i] = new uint8_t*[matrix_width];
        for (int j = 0; j < matrix_width; j++)
        {
            //uint8_t** data_array2D = data_array3D[i];
            data_array3D[i][j] = new uint8_t[3]; 
            for(int k = 0; k < 3; k++){
                data_array3D[i][j][k] = 0;
            }
        }
    }

	if (data_array3D == nullptr) {
        dataArrayLength = 0;
        return false;
	}
    return true;
}

/**
 * @brief Remove led matrix from heap
 */
void DStripData::Destroy(void)
{
    if (data_array3D != nullptr)
    {
        for(int i = 0; i != matrix_height; ++i)
        {
            for(int j = 0; j != matrix_width; ++j)
            {
                delete[] data_array3D[i][j];
            }
            delete[] data_array3D[i];
        }
    delete[] data_array3D;
    }
    data_array3D = nullptr;
    dataArrayLength = 0;
}
