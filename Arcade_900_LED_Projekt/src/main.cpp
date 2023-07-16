#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_system.h>
#include <esp_http_server.h>
#include <esp_partition.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <vector>

#include "Othello/Othello_main.h"
#include "Snake/snake_main.h"

#include "Webserver.h"
#include "Fileserver.h"
#include "driver/rmt.h"
#include "soc/rmt_struct.h"

#include "ESP32RMTChannel.h"
#include "DStripData.h"
#include "DLEDController.h"
#include "Pixel.h"

#include "sdkconfig.h"

#define MATRIX_LENGTH 30
#define MATRIX_HEIGHT 30

static const uint8_t  cfgOutputPin = 27;    // the GPIO where LEDs are connected
static const uint8_t  cfgChannel   = 0;     // ESP32 RMT's channel [0 ... 7]
static const uint16_t cfgLEDcount  = MATRIX_LENGTH * MATRIX_HEIGHT;    // 900 LEDS
const uint8_t  cfgMaxCCV    = 255;    // maximum value allowed for color component
const TickType_t xBlockTime = 10000 / portTICK_PERIOD_MS; // set timeout to 10 seconds

void SetMutex(SemaphoreHandle_t);

SemaphoreHandle_t mutex3DArray;
DLEDController LEDcontroller;


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        printf("connected\n");
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        printf("disconntected\n");
    }
}


void Test(Lobby * lobby){
    printf("start Buttontask\n");
    while(true){
        printf("start buttonloop\n");
        for(int i = 0; i < 4; i++){
            if(lobby->getPlayer(i)->getconnected()){
                if(lobby->getPlayer(i)->Buttons[0].getButton()){
                    //xTaskCreate(DisplayTask, "Display task", 2048, NULL, uxTaskPriorityGet(NULL) + 5, &xHandleDisplayTask);
                    printf("button Player %d\n", i);
                    lobby->getPlayer(i)->Buttons[0].setButton(false);
                }
            }
        }
        if(lobby->getState() == LobbyState::lobby_closed){
            return;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void square(Lobby * lobby, DLEDController * LED){
    Player * p = lobby->getPlayer(0);
    while(true){
        if(p->Buttons[A].getButton()){
            p->Buttons[A].setButton(false);
            printf("A pressed\n");
            LED->stripData.data_array3D[0][0][red_byte] = 255;
            LED->stripData.data_array3D[0][0][blue_byte] = 0;
            LED->stripData.data_array3D[0][0][green_byte] = 0;
        }
        if(p->Buttons[Up].getButton()){
            p->Buttons[Up].setButton(false);
            printf("Up pressed\n");
            LED->stripData.data_array3D[0][0][red_byte] = 0;
            LED->stripData.data_array3D[0][0][blue_byte] = 0;
            LED->stripData.data_array3D[0][0][green_byte] = 0;
        }
        LED->SetLEDs();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    } 
}

void Idle_Screen(DLEDController * LED){
    int i = 0, h=29;
    for (int j = 0; j < 30; j++){
        for (int k = 1; k < 3; k++){
                for(int c=1; c<=255; c++){
                    LEDcontroller.stripData.data_array3D[i][j][red_byte] = c;
                    LEDcontroller.stripData.data_array3D[i][j][blue_byte] = c;
                    LEDcontroller.stripData.data_array3D[i][j][green_byte] = c;
                    LEDcontroller.stripData.data_array3D[h][j][red_byte] = c;
                    LEDcontroller.stripData.data_array3D[h][j][blue_byte] = c;
                    LEDcontroller.stripData.data_array3D[j][i][red_byte] = c;
                    LEDcontroller.stripData.data_array3D[j][i][green_byte] = c;
                    LEDcontroller.stripData.data_array3D[j][h][green_byte] = c;
                    LEDcontroller.stripData.data_array3D[j][h][blue_byte] = c;
                } 
                       
        }
        LED->SetLEDs();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
   
    for (int j = 0; j < 30; j++){
        for (int k = 1; k < 3; k++){
            for(int z=255; z>=1; z--){
                    LEDcontroller.stripData.data_array3D[i][j][red_byte] = z;
                    LEDcontroller.stripData.data_array3D[i][j][green_byte] = z;
                    LEDcontroller.stripData.data_array3D[h][j][red_byte] = z;
                    LEDcontroller.stripData.data_array3D[j][i][green_byte] = z;
                    LEDcontroller.stripData.data_array3D[j][h][blue_byte] = z;
                    }
                    
                }   
                LED->SetLEDs();
                vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        
    
}

void GameTask(void * l){
    Lobby * lobby = ((LobbyandLED*)l)->lobby;
    DLEDController * LED = ((LobbyandLED*)l)->LEDcontroller;
    while(true){
        printf("GameTaskIDLE\n");
        Idle_Screen(LED);
        if(lobby->getState() == LobbyState::game_running){
            //switch for starting game based on ID 
            switch(lobby->getGameID()){
                case 0:{
                    Test(lobby);
                    break;
                }
                case 1:{
                    square(lobby, LED);
                    break;
                }  
                case 2:{
                    Othello_main(lobby, LED);
                    break;
                }
                case 3:{
                    Snake_main(lobby);
                    break;
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.wifi_task_core_id = 0;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "Arcade_LED",
            .password = "",
            .ssid_len = strlen("Arcade_LED"),
            .channel = 1,
            .authmode = WIFI_AUTH_OPEN,
            .ssid_hidden = 0,
            .max_connection = 10
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_wifi_start());

    const char * base_path = "/data";

    mount_storage(base_path);


    Lobby * lobby = new Lobby{};
    start_webserver(base_path, lobby);

    
    LEDcontroller.stripData.Create(cfgLEDcount);
    LEDcontroller.rmtChannel.Initialize((rmt_channel_t)cfgChannel, (gpio_num_t)cfgOutputPin, 900 * 3 * 8); // Changed originally * 24
    LEDcontroller.rmtChannel.ConfigureForWS2812x();
    LEDcontroller.SetLEDType(LEDType::WS2812B);
    LEDcontroller.SetLEDs();
    vTaskDelay(500 / portTICK_PERIOD_MS);

    LobbyandLED * lobbyandLED = new LobbyandLED{lobby, &LEDcontroller};
    xTaskCreatePinnedToCore(&GameTask, "GameTask", 50000, lobbyandLED, 20, NULL, 1);
}