#include "DStripData.h"
#include <esp_log.h>

extern DStripData stripData;
extern const uint8_t cfgMaxCCV;

extern "C" {

    void SetPixelPWM(uint16_t index){
        uint8_t* data = stripData.Data();
        // find exact location in data memory array
        data += index * stripData.BytesPerLED(); 
        // make LED first blank
        *data = 255; data++; 
        *data = 255; data++;
        *data = 255; data++;
        *data = 255; data++;
    }
    void SetPixel(uint8_t row, uint8_t col , uint8_t colNumbers, uint8_t r, uint8_t g, uint8_t b){
        if (stripData.BytesPerLED() != 4) {
            ESP_LOGE("colorutils.h", "stripData's bytesPerLED should be 4 !");
            return;
        }

        if (stripData.Data() == nullptr) return;
        if (row * colNumbers + col >= stripData.StripLength()) return;        
        

        uint8_t* data = stripData.Data();
//        data += idx * stripData.BytesPerLED();
        data += (row * col + colNumbers) * stripData.BytesPerLED();
        *data = 0; data++;
        *data = r; data++;
        *data = g; data++;
        *data = b;
    }
}

    void SetPixel(uint16_t idx, uint8_t r, uint8_t g, uint8_t b)
    {
        if (stripData.BytesPerLED() != 4) {
            ESP_LOGE("colorutils.h", "stripData's bytesPerLED should be 4 !");
            return;
        }

        if (stripData.Data() == nullptr) return;
//        if (row * col + colNumbers >= stripData.StripLength()) return;        
        if (idx >= stripData.StripLength()) return;

        uint8_t* data = stripData.Data();
        data += idx * stripData.BytesPerLED();

        *data = 0; data++;
        *data = r; data++;
        *data = g; data++;
        *data = b;
    }

    void SetColorByIndex(uint16_t pidx, uint16_t colorIdx)
    {
        uint8_t cc = cfgMaxCCV;
        uint8_t seq = (colorIdx / cc) % 6;
        uint8_t idx =  colorIdx % cc;

        switch (seq) {
            case 0: SetPixel(pidx, cc,       idx,      0);        break;
            case 1: SetPixel(pidx, cc - idx, cc,       0);        break;
            case 2: SetPixel(pidx, 0,        cc,       idx);      break;
            case 3: SetPixel(pidx, 0,        cc - idx, cc);       break;
            case 4: SetPixel(pidx, idx,      0,        cc);       break;
            case 5: SetPixel(pidx, cc,       0,        cc - idx); break;
        }
    }

    void RainbowStep(uint16_t step)
    {
        for (uint16_t i = 0; i < stripData.StripLength(); ++i)
            SetColorByIndex(i, i + step);
    }

    void MovePixel(uint16_t step)
    {
        uint8_t *data = stripData.Data();
        uint16_t stripLen = stripData.StripLength();
        if (data == nullptr) return;
        if (stripLen == 0) return;

        if (stripData.BytesPerLED() != 4) {
            ESP_LOGE("colorutils.h", "stripData's bytesPerLED should be 4 !");
            return;
        }
                        
        uint8_t  seq = (step / stripLen) % 30;  // (0 / 900) % 30
        uint16_t idx =  step % stripLen;   // 0 / 900 --> the LED we are on
        uint8_t r, g, b, c1, c2;

        c1 = cfgMaxCCV;     // 255
        c2 = c1 / 2;        // 127

        switch (seq) {
            // first, colour is red
            case 0:  r = c1; g =  0; b =  0; break;
            // then, colour is green
            case 1:  r =  0; g = c1; b =  0; break;
            // then colour is blue
            case 2:  r =  0; g =  0; b = c1; break;
            case 3:  r = c2; g = c2; b =  0; break;
            case 4:  r =  0; g = c2; b = c2; break;
            default: r = c2; g =  0; b = c2; break;
        }
            // when moving LED is descending
        if (seq & 0x01) {
            idx = stripLen - idx - 1;
        }

        for (uint16_t i = 0; i < stripLen; ++i) {
            if (i == idx) {
                *data = 0; data++;
                *data = r; data++;
                *data = g; data++;
                *data = b; data++;
            }
            else {
                *data = *data / 2; data++;
                *data = *data / 2; data++;
                *data = *data / 2; data++;
                *data = *data / 2; data++;
            }
        }
    }

