/* Esptouch example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "button.h"
#include "wifi.h"
#include "nvs.h"

static const char *TAG = "main";

//todo store wifi to smartconfig after successful configuration
void app_main(void)
{
    nvs_init();
    int mode_button_state = read_mode_button();
    ESP_LOGI(TAG, "mode button state %d\n", mode_button_state);

    // led_init();
    if (mode_button_state == 1)
    {
        wifi_init_smartconfig();
    }
    else
    {
        wifi_init_sta();
    }
}