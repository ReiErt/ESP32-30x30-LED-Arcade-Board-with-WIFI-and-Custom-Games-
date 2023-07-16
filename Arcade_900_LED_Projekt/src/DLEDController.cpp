#define matrix_width 30
#define matrix_height 30
#define red_byte 0
#define green_byte 1
#define blue_byte 2
#include <stdio.h>
#include "DLEDController.h"
#include "ESP32RMTChannel.h"
#include "DStripData.h"

const TickType_t xBlockTime = 10000 / portTICK_PERIOD_MS; // set timeout to 10 seconds


DLEDController::DLEDController(void)
{
    type = LEDType::notset;
    colorOrder = LEDColorOrder::Flat;
    bytesPerLED = 0;

    mutex = NULL;
}

DLEDController::~DLEDController(void)
{
    //
}

bool DLEDController::SetLEDType(LEDType newtype)
{
    bytesPerLED = 0; // bytesPerLED != 0 is the marker for initialization

    type = newtype;

    return SetProperties();
}

bool DLEDController::SetProperties(void)
{
    /* Timings are from datasheets */

//reset code = wsTRS >= 50 us = 50000 ns
//according to manufacturer website, T0H = 0,4us | T1H = 0,8us | T0L = 0,85us | T1L = 0,45 us | EVERYTHING +- 150ns
    if (type == LEDType::WS2812){
        wsT0H = 350; wsT0L = 800; wsT1H = 700; wsT1L = 600; wsTRS = 50000;
        colorOrder = LEDColorOrder::GRB;
        bytesPerLED = 3;
        SetTimingsForRMT();
    }
    else if(type == LEDType::WS2812B){
        wsT0H = 300; wsT0L = 1090; wsT1H = 1090; wsT1L = 320; wsTRS = 280000;
        colorOrder = LEDColorOrder::GRB;
        bytesPerLED = 3;
        SetTimingsForRMT();
    }
    else {
        wsT0H = 0; wsT0L = 0; wsT1H = 0; wsT1L = 0; wsTRS = 0;
        colorOrder = LEDColorOrder::Flat;
        bytesPerLED = 0;
        printf("something went wrong");
    }
    return (bytesPerLED == 0) ? false : true;
}

void DLEDController::SetTimingsForRMT(void)
{
    // ESP32 RMT, as configured by ESP32RMTChannel.ConfigureForWS2812x uses a RMT clock duration of 50 ns.
    // RMT has 20 MHz clock = 50ns
    uint16_t rmt_clk_duration = 50;

    //level0 is found at rmt_item32_t(16)
	rmtLO.level0 = 1;
    //level1 is found at rmt_item32_t(0)
	rmtLO.level1 = 0;
    // how many clock cycles are encoded into first 15 MSBs of rmt_item32_t
	rmtLO.duration0 = wsT0H / rmt_clk_duration; // 7 
    // how many clock cycles are encoded into rmt_item32_t(15 downto 1)
	rmtLO.duration1 = wsT0L / rmt_clk_duration; // 16

	rmtHI.level0 = 1;
	rmtHI.level1 = 0;
	rmtHI.duration0 = wsT1H / rmt_clk_duration; // 14
	rmtHI.duration1 = wsT1L / rmt_clk_duration; // 12

	rmtLR.level0 = 1;
	rmtLR.level1 = 0;
	rmtLR.duration0 = wsT0H / rmt_clk_duration;
	rmtLR.duration1 = wsTRS / rmt_clk_duration;

	rmtHR.level0 = 1;
	rmtHR.level1 = 0;
	rmtHR.duration0 = wsT1H / rmt_clk_duration;
	rmtHR.duration1 = wsTRS / rmt_clk_duration;
}

// Author Lukas Landgraf
// setzt LEDBuffer auf 0, 0, 0
void DLEDController::ClearLEDBuffer(){
    for(uint8_t i = 0; i < 30; i++){
        for(uint8_t j = 0; j < 30; j++){
            setDataArray(i, j, 0, 0, 0);
        }
    }
}

// Author Lukas Landgraf
// mit den x und y koordinaten nimmt das Symbol 3 LEDs nach rechts und 5 nach oben ein
// r g b für Farbe und c für symbol
// Ursprünglich für text ausgabe in Othello gedacht
// kann alle Zahlen ausgeben
// begrenzte anzahl an Buchstaben
void DLEDController::SymbolOnLED(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, char c){
    switch(c){
        case ':':{
            setDataArray(x + 1, y - 1, r, g, b);
            setDataArray(x + 1, y - 3, r, g, b);
            break;
        }
        case '0':{
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            break;
        }
        case '1':{
            setDataArray(x, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case '2':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case '3':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b); 
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case '4':{
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case '5':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case '6':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case '7':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y - 1, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case '8':{
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            break;
        }
        case '9':{
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case 'b':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case 'c':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            break;
        }
        case 'e':{
            setDataArray(x, y, r, g, b);
            break;
        }
        case 'i':{
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            break;
        }
        case 'o':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            break;
        }
        case 'h':{
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            break;
        }
        case 'A':{
            setDataArray(x, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            break;
        }
        case 'S':{
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            break;
        }
        case 'H':{
            setDataArray(x, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case 'O':{
            setDataArray(x, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            break;
        }
        case 'C':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case 'E':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 4, r, g, b);
            break;
        }
        case 'P':{
            setDataArray(x, y, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            break;
        }
        case 'B':{
            setDataArray(x, y, r, g, b);
            setDataArray(x + 1, y, r, g, b);
            setDataArray(x + 2, y - 1, r, g, b);
            setDataArray(x, y - 1, r, g, b);
            setDataArray(x, y - 2, r, g, b);
            setDataArray(x + 1, y - 2, r, g, b);
            setDataArray(x + 2, y - 3, r, g, b);
            setDataArray(x, y - 3, r, g, b);
            setDataArray(x, y - 4, r, g, b);
            setDataArray(x + 1, y - 4, r, g, b);
            break;
        }
    }
}

void DLEDController::SetLEDs()
{
    if (bytesPerLED == 0) return;
    uint8_t*** data_array = stripData.get_DataArray();
    uint16_t array_length = stripData.ArrayDataLength();
    if (data_array == nullptr) return;
    if (array_length == 0) return;

    channelBuffer = rmtChannel.GetDataBuffer();
    if (channelBuffer == nullptr) return;


    channelBufLen = rmtChannel.GetDataBufferLen();
    if (channelBufLen == 0) return;

    if (mutex != NULL) 
    {
        // take mutex. block process
        if (xSemaphoreTake(mutex, xBlockTime) == pdFALSE) {
            // failed to take the mutex in 'xBlockTime', maybe at the next call ?
            return;
        }
    }
     channelIndex = 0;

    
    int8_t k = 29;
    uint8_t l;
    // i = 29
    for(int8_t i = matrix_height-1; i != -1; --i){

        // j = 29
        for (int8_t j = matrix_height-1; j != -1; --j)
        {
            // fragen ab ob row == ungerade
            if (i%2 != 0){
                l = k-j;
                // j = 0
                SetRMTItemsFromByte(data_array[i][l]+green_byte);   // blue
                SetRMTItemsFromByte(data_array[i][l]+red_byte);  // green
                SetRMTItemsFromByte(data_array[i][l]+blue_byte);    // red 
            }

            if (i%2 == 0){
                SetRMTItemsFromByte(data_array[i][j]+green_byte);   // blue
                SetRMTItemsFromByte(data_array[i][j]+red_byte);  // green
                SetRMTItemsFromByte(data_array[i][j]+blue_byte);    // red 
            
            }
        }
    }
    
    if (mutex != NULL) {
        // give mutex back. Unblock
        xSemaphoreGive(mutex);
    }
    if (channelBuffer[channelIndex].val == rmtHI.val) {
        channelBuffer[channelIndex] = rmtHR;
    }
    else {
        channelBuffer[channelIndex] = rmtLR;
    }
    rmtChannel.SendData();
}
    
void DLEDController::SetRMTItemsFromByte(uint8_t* value)
{
    // 0x80 = 0b10000000
	uint8_t mask = 0x80;
	while (mask != 0){
        if (channelIndex < channelBufLen)
            channelBuffer[channelIndex++] = ((*value & mask) != 0) ? rmtHI : rmtLO;
		mask = mask >> 1;
	}
}
void DLEDController::iterateVectorto3DArray(std::vector<Pixel *>& pixelVector) {

    for (auto& item : pixelVector) {
        //item.moveto3DArray(stripData);
        // transfers the RGB value in array to X,Y location on 3D array
        *(*(*(stripData.data_array3D+item->x_location)+item->y_location)+0)  = item->pixel_data[0];
        *(*(*(stripData.data_array3D+item->x_location)+item->y_location)+1)  = item->pixel_data[1];
        *(*(*(stripData.data_array3D+item->x_location)+item->y_location)+2)  = item->pixel_data[2];
    }
}

