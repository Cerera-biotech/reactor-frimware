#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "init_config.h"
#include "nvs.h"

static const char *init_CONFIG_TAG = "config_counter";

#define REBOOTS_TO_START_CONFIG 4
#define TIME_BEFORE_COUNTER_RESET_MS 5000

bool increment_smart_config_counter()
{
    uint8_t counter = nvs_get_smart_config_counter();
    ESP_LOGI(init_CONFIG_TAG, "incrementing counter %d", counter);
    counter++;
    nvs_set_smart_config_counter(counter);
    return counter >= REBOOTS_TO_START_CONFIG;
}

void reset_smart_config_counter_task()
{
    ESP_LOGI(init_CONFIG_TAG, "reset counter task started");
    vTaskDelay(TIME_BEFORE_COUNTER_RESET_MS / portTICK_PERIOD_MS);
    ESP_LOGI(init_CONFIG_TAG, "reset counter");
    nvs_set_smart_config_counter(0);
    vTaskDelete(NULL);
}

void init_smart_config_counter_task()
{    
    xTaskCreate(reset_smart_config_counter_task, "counter_reset", 4096, NULL, 3, NULL);
}