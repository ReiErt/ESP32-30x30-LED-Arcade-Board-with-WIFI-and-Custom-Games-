#ifndef ESP32RMTChannel_H
#define ESP32RMTChannel_H

#include <stdint.h>
#include "driver/rmt.h"
#include "soc/rmt_struct.h"

class ESP32RMTChannel
{
public:
    ESP32RMTChannel(void);
    ~ESP32RMTChannel(void);

    /**
    @brief Initialize onboard rmt (remote control receiver) properties
    For WS2812B LEDs, the length of data buffer is 24 bits. (8 bit * bytesperLED)
    Every uint8_t bit in DStripData is converted into rmt_item32_t
    */
    bool Initialize(rmt_channel_t rmt_channel, gpio_num_t gpio_pin, uint32_t numberOfItems);

    /**
     * @brief Clear the channel properties
     */
    void Cleanup(void);

    /**
     * @brief Initialise rmt channel to send WS2812B-specific timing signals
     * Function sets RMT clock to 50 ns.
     */
    bool ConfigureForWS2812x(void);

    /**
      @brief Sends the data
     * When function rmt_write_items(); is set to true, it blocks all other functions from CPU until finished
     */
    bool SendData(void);

    // Return buffer
    rmt_item32_t* GetDataBuffer(void);
    uint32_t      GetDataBufferLen(void);

    /**
     * @brief Set an item of data buffer
     * Safer but slower way to set the content of the data buffer.
     */
    void SetDataBuffer(uint32_t index, rmt_item32_t value);

protected:
    bool initialized;
    rmt_item32_t  data[900 * 24];     // Pointing to beginning of heap buffer. This buffer is sent over rmt
	uint32_t      length;
    rmt_channel_t channel;
    gpio_num_t    pin;
    bool          driverInstalled;

	/**
	 * @brief Create data buffer in heap
     *
	 * @param numberOfItems The length of data buffer.
	 */
    bool CreateBuffer(uint32_t numberOfItems);

    /**
     * @brief Garbage collector
     */
    void DestroyBuffer(void);

    /**
     * @brief Set GPIO as output
     *
     * @param level Output level - use 0 for low and 1 for high.
     */
    bool SetGPIO_Out(uint32_t level);

    /**
     * @brief Install RMT driver on channel
     */
    bool InstallDriver(void);

    /**
     * @brief Uninstall RMT peripheral for given channel
     */
    bool UninstallDriver(void);
};

#endif
