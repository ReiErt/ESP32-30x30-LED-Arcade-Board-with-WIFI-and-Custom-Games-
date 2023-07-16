// #ifndef DLEDController_H
// #define DLEDController_H
#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include <freertos/semphr.h>
#include "driver/rmt.h"

#include "DStripData.h"
#include "ESP32RMTChannel.h"
#include <vector>
#include "Pixel.h"



/**
* @brief digial LED type
*/
enum class LEDType
{
    notset,
    WS2812,
    WS2812B,
};

/**
* @brief The color order used by digital LEDs
*/
enum class LEDColorOrder
{
    Flat,           /* default order */
    GRB,            /* WS2812B */
};

// -----------------------------------------------------------------------------

class DLEDController
{
public:
    DLEDController(void);
    ~DLEDController(void);

    bool SetLEDType(LEDType newtype);

    void SetMutex(SemaphoreHandle_t);

    void SetLEDs();

    void iterateVectorto3DArray(std::vector<Pixel *>& pixelVector);

    void ClearLEDBuffer();
    void SymbolOnLED(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, char c);

    // Author Lukas Landgraf
    // weniger schreibarbeit um data_array3D mit Farben zu füllen
    inline void setDataArray(uint8_t x, uint8_t y, uint8_t r, uint8_t g, int8_t b){

        stripData.data_array3D[y][x][red_byte] = r;
        stripData.data_array3D[y][x][green_byte] = g;
        stripData.data_array3D[y][x][blue_byte] = b;
    };
   
    DStripData stripData;
    ESP32RMTChannel rmtChannel;

protected:
    LEDType type;
    SemaphoreHandle_t mutex;

    /**
    * @brief Set the properties based on LEDType
    */
    bool SetProperties(void);

    LEDColorOrder colorOrder;
    uint8_t       bytesPerLED;

    uint16_t wsT0H, wsT0L; // timing of analoge signal in nanoseconds for WS2812B to read logical 0
    uint16_t wsT1H, wsT1L; // timing of analoge signal in nanoseconds for WS2812B to read logical 1
    uint32_t wsTRS;        // timing of analoge signal in nanoseconds for WS2812B to reset

    /**
    *@brief Set ESP32 RMT timings to output logical 0 and 1
    RMT clock cycles at 20MHz or 50 ns
     */
    void SetTimingsForRMT(void);
    
	rmt_item32_t rmtLO, rmtHI; // Low and high signal. rmtLO = wsT0H + wsT0L
    rmt_item32_t rmtLR, rmtHR; // Signals for reset. rmtHI = wsT1H + wsT1L

    uint32_t channelBufLen;
    rmt_item32_t *channelBuffer; // an encoded 32 bit datatype, which the ws2812B reads as either logical 0 or 1
    rmt_item32_t **arrayChannelBuffer; // an encoded 32 bit datatype, which the ws2812B reads as either logical 0 or 1
    uint32_t channelIndex;

    /**
     * @brief Set rmt_items32_t items
     * This function reads datastrip buffer and converts set bits to rmtHI of type rmt_items32_t and 
     * not set bits to rmtLO of type rmt_item32_t and places RMT items in channelBuffer
     */
   	void SetRMTItemsFromByte(uint8_t* value);

private:

};

