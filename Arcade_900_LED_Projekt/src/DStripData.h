#pragma once
#include <stddef.h>
#include <stdint.h>


class DStripData
{
public:
    
    DStripData(void);
    virtual ~DStripData();
    bool Create(uint16_t numberOfLEDs);
    void Destroy(void);

    uint16_t get_numLEDs(void)     { return numLEDs;        }
    uint8_t*** get_DataArray(void) { return data_array3D;   }
    uint16_t ArrayDataLength(void) { return dataArrayLength;}
    uint8_t get_bytesPerLED(void)  { return bytesPerLED;    }

    uint16_t            numLEDs;
    uint8_t   ***data_array3D;
    uint16_t            dataArrayLength;
    const uint8_t       bytesPerLED = 4;
   
protected:
private:
};

