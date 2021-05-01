#pragma once
#include "nvs_flash.h"
#include "nvs.h"

static const char *NVS_TAG = "nvs";

void hanlde_nvs_err(esp_err_t err)
{
    switch (err)
    {
    case ESP_OK:
        ESP_LOGI(NVS_TAG, "got value");
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGE(NVS_TAG, "The value is not initialized yet!\n");
        break;
    default:
        ESP_LOGE(NVS_TAG, "Error (%s) reading!\n", esp_err_to_name(err));
    }
}

void nvs_init()
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

//todo add errors reporting here
bool nvs_get_smart_config_flag()
{
    bool value;
    // Open
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_get_i8(my_handle, "smart_config", &value);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
    return value;
}

//todo add errors reporting here
void nvs_set_smart_config_flag(bool value)
{

    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_i8(my_handle, "smart_config", value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }
}

//todo add errors reporting here
uint32_t nvs_get_ch_value(uint8_t ch)
{
    char key[10];
    sprintf(key, "ch_%d", ch);
    uint32_t value = 0;
    // Open
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(NVS_TAG, "reading ch %s value", key);
        err = nvs_get_u32(my_handle, key, &value);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
    return value;
}

//todo add errors reporting here
void nvs_set_ch_value(uint8_t ch, uint32_t value)
{
    char key[10];
    sprintf(key, "ch_%d", ch);
    // Open
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(NVS_TAG, "trying to write ch %s value %d to nvs", key, value);
        err = nvs_set_u32(my_handle, key, value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }
}
