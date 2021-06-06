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
#include "wifi.h"
#include "nvs.h"
#include "init_config.h"
#include "version.h"
#include "adc.h"
#include "ph_service.h"
// #include <ads111x.h>

static const char *TAG = "main";

//todo store wifi to smartconfig after successful configuration
void app_main(void)
{
    print_welcome();
    nvs_init();
    bool smartconfig = increment_smart_config_counter();

    led_init();
    start_measuring();
    start_temperature_sensor();
    start_ph_service();

    init_smart_config_counter_task();
    if (smartconfig)
    {
        wifi_init_smartconfig();
    }
    else
    {
        test_leds();
        set_leds_from_nvs();
        wifi_init_sta();
    }
}