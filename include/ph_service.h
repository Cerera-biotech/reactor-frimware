#pragma once
#include "adc.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs.h"

const char *phTAG = "ph_service";

float ph_service_min_ph, ph_service_max_ph = 0;
float ph_service_temperature = 25;
float ph_service_neutralVoltage_mv = 1200; // ph 4.0
float ph_service_acidVoltage_mv = 2100;    // ph 7.0

float readPH(float voltage, float temperature)
{
    float slope = (7.0 - 4.0) / ((ph_service_neutralVoltage_mv - 1500.0) / 3.0 - (ph_service_acidVoltage_mv - 1500.0) / 3.0); // two point: (ph_service_neutralVoltage_mv,7.0),(ph_service_acidVoltage_mv,4.0)
    float intercept = 7.0 - slope * (ph_service_neutralVoltage_mv - 1500.0) / 3.0;
    float _phValue = slope * (voltage - 1500.0) / 3.0 + intercept; //y = k*x + b
    return _phValue;
}

void handle_ph(float ph)
{
    ESP_LOGI(phTAG, "handling PH: %f; min: %f; max: %f", ph, ph_service_min_ph, ph_service_max_ph);
    if (ph > ph_service_max_ph)
    {
        //todo turn off c02
        ESP_LOGI(phTAG, "need to turn off the valve");
    }
    else if (ph < ph_service_min_ph)
    {
        //todo turn on co2
        ESP_LOGI(phTAG, "need to turn on the valve");
    }
    else
    {
        ESP_LOGI(phTAG, "ph is ok");
        // do nothing
        // todo add some regulating coeficient
    }
}

// Main task
void ph_task(void *pvParameters)
{
    while (1)
    {
        float voltage = 0;
        if (adc_read_0(&voltage))
        {
            ESP_LOGI(phTAG, "got ph voltage %f", voltage);
            handle_ph(readPH(voltage * 1000, ph_service_temperature));
        }
        else
        {
            ESP_LOGE(phTAG, "failed to get PH");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void set_ph_levels(float min, float max)
{
    ph_service_max_ph = max;
    ph_service_min_ph = min;
}

void set_ph4_level()
{
    float voltage = 0;
    if (adc_read_0(&voltage))
    {
        ESP_LOGI(phTAG, "got ph4 calibration voltage %f", voltage);
        nvs_set_ph4_mv(voltage * 1000);
        ph_service_neutralVoltage_mv = voltage * 1000;
    }
    else
    {
        ESP_LOGE(phTAG, "failed to get ph4 calibration");
    }
}

void set_ph7_level()
{
    float voltage = 0;
    if (adc_read_0(&voltage))
    {
        ESP_LOGI(phTAG, "got ph7 calibration voltage %f", voltage);
        nvs_set_ph7_mv(voltage * 1000);
        ph_service_acidVoltage_mv = voltage * 1000;
    }
    else
    {
        ESP_LOGE(phTAG, "failed to get ph7 calibration");
    }
}

void ph_service_init_values()
{
    set_ph_levels(nvs_get_min_ph(), nvs_get_max_ph());
    uint32_t voltage = 0;
    if (nvs_get_ph4_mv(&voltage))
    {
        ph_service_neutralVoltage_mv = voltage;
    }
    else
    {
        ESP_LOGE(phTAG, "failed to get ph4 calibration value set as default");
        ph_service_neutralVoltage_mv = 1200;
    }
    if (nvs_get_ph7_mv(&voltage))
    {
        ph_service_acidVoltage_mv = voltage;
    }
    else
    {
        ESP_LOGE(phTAG, "failed to get ph7 calibration value set as default");
        ph_service_acidVoltage_mv = 2100;
    }
}

void start_ph_service()
{
    ph_service_init_values();
    // // Clear device descriptors
    // memset(device, 0, sizeof(device));
    // Start task
    xTaskCreate(ph_task, "ph_service", 5000, NULL, 12, NULL);
}