#pragma once
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ds18x20.h>
#include <esp_log.h>
#include <esp_err.h>
#include "pin_config.h"

// Use address of your own sensor here!
// You can find out the address of your sensor by running ds18x20_multi example
static const ds18x20_addr_t SENSOR_ADDR = 0x0333;

static const char *tempTAG = "ds18x20_svc";
float temperature_service_temp = 25.0;
ds18x20_addr_t addrs[1];
void test(void *pvParameter)
{
    // Make sure that the internal pull-up resistor is enabled on the GPIO pin
    // so that one can connect up a sensor without needing an external pull-up.
    // (Note: The internal (~47k) pull-ups of the ESP do appear to work, at
    // least for simple setups (one or two sensors connected with short leads),
    // but do not technically meet the pull-up requirements from the ds18x20
    // datasheet and may not always be reliable. For a real application, a proper
    // 4.7k external pull-up resistor is recommended instead!)
    gpio_set_pull_mode(TEMP_SENSOR_GPIO, GPIO_PULLUP_ONLY);

    esp_err_t res;
    while (1)
    {
        res = ds18x20_measure_and_read(TEMP_SENSOR_GPIO, addrs[0], &temperature_service_temp);
        if (res != ESP_OK)
            ESP_LOGE(tempTAG, "Could not read from sensor %08x%08x: %d (%s)",
                     (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, res, esp_err_to_name(res));
        else
            ESP_LOGI(tempTAG, "Sensor %08x%08x: %.2f°C",
                     (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, temperature_service_temp);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

float get_temperature()
{
    return temperature_service_temp;
}

void start_temperature_sensor()
{
    // Every RESCAN_INTERVAL samples, check to see if the sensors connected
    // to our bus have changed.
    size_t sensor_count;
    esp_err_t res = ds18x20_scan_devices(TEMP_SENSOR_GPIO, addrs, 1, &sensor_count);
    if (res != ESP_OK)
    {
        ESP_LOGE(tempTAG, "Sensors scan error %d (%s)", res, esp_err_to_name(res));
        return;
    }

    if (!sensor_count)
    {
        ESP_LOGW(tempTAG, "No sensors detected!");
        return;
    }

    xTaskCreate(test, tempTAG, configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL);
}