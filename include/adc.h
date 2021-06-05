#pragma once

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ads111x.h>
#include <string.h>

#define I2C_PORT 0

#define SDA_GPIO 19
#define SCL_GPIO 18

#if defined(CONFIG_IDF_TARGET_ESP32S2)
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define GAIN ADS111X_GAIN_4V096   // +-4.096V
#define ADC_ADDR ADS111X_ADDR_VCC // ADS111X_ADDR_VCC ADS111X_ADDR_GND

// Gain value
static float gain_val;

i2c_dev_t device;

static bool measure(uint8_t ch, uint8_t ch_to_log, float *result)
{

    ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ch));
    // wait for conversion end
    bool busy;
    do
    {
        ads111x_is_busy(&device, &busy);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        // printf("ADC busy\n");
    } while (busy);

    // Read result
    int16_t raw = 0;
    if (ads111x_get_value(&device, &raw) == ESP_OK)
    {
        *result = gain_val / ADS111X_MAX_VALUE * raw;
        printf("Raw ADC ch%d value: %d, voltage: %.04f volts\n", ch_to_log, raw, *result);
        return true;
    }
    printf("Cannot read ADC value\n");
    return false;
}

// Main task
void ads111x_test(void *pvParameters)
{
    while (1)
    {
        float result;
        measure(ADS111X_MUX_3_GND, 0, &result);
        measure(ADS111X_MUX_0_GND, 1, &result);
        // measure(ADS111X_MUX_1_GND, 2, &result);
        // measure(ADS111X_MUX_2_GND, 3, &result);
        printf("\n");

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void start_measuring()
{
    // Init library
    ESP_ERROR_CHECK(i2cdev_init()); //move it somewhere

    gain_val = ads111x_gain_values[GAIN];

    ESP_ERROR_CHECK(ads111x_init_desc(&device, ADC_ADDR, I2C_PORT, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(ads111x_set_mode(&device, ADS111X_MODE_CONTINUOUS));  // Continuous conversion mode
    ESP_ERROR_CHECK(ads111x_set_data_rate(&device, ADS111X_DATA_RATE_8)); // 32 samples per second
    // ESP_ERROR_CHECK(ads111x_set_input_mux(&device, ADS111X_MUX_0_GND));   // positive = AIN0, negative = GND
    ESP_ERROR_CHECK(ads111x_set_gain(&device, GAIN));

    // // Clear device descriptors
    // memset(device, 0, sizeof(device));
    // Start task
    // xTaskCreate(ads111x_test, "adc_test_task", 5000, (void *)AF_INET, 12, NULL);
}

bool adc_read_0(float *result)
{
    return measure(ADS111X_MUX_0_GND, 1, result);
}

bool adc_read_1(float *result)
{
    return measure(ADS111X_MUX_1_GND, 2, result);
}
