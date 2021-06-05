#pragma once
#include "nvs_flash.h"

static const char *NVS_TAG = "nvs";

bool hanlde_nvs_err(esp_err_t err)
{
    switch (err)
    {
    case ESP_OK:
        ESP_LOGI(NVS_TAG, "got value");
        return true;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGE(NVS_TAG, "The value is not initialized yet!\n");
        break;
    default:
        ESP_LOGE(NVS_TAG, "Error (%s) reading!\n", esp_err_to_name(err));
    }
    return false;
}

float round(float var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =3767.16    for rounding off value
    // then type cast to int so value is 3767
    // then divided by 100 so the value converted into 37.67
    float value = (int)(var * 100 + .5);
    ESP_LOGI(NVS_TAG, "rounding %f, %f, %f", var, value, (float)value / 100);
    return (float)value / 100;
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
uint8_t nvs_get_smart_config_counter()
{
    uint8_t value;
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
        err = nvs_get_u8(my_handle, "smart_config", &value);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
    return value;
}

//todo add errors reporting here
void nvs_set_smart_config_counter(uint8_t value)
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
        err = nvs_set_u8(my_handle, "smart_config", value);
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

float nvs_ph_multiplier = 100.0;

//todo add errors reporting here
float nvs_get_min_ph()
{
    uint32_t value;
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
        ESP_LOGI(NVS_TAG, "reading min_ph");
        err = nvs_get_u32(my_handle, "min_ph", &value);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
    return round(value / nvs_ph_multiplier);
}

//todo add errors reporting here
void nvs_set_min_ph(float value)
{
    uint32_t to_set = value * nvs_ph_multiplier;
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
        ESP_LOGI(NVS_TAG, "trying to write min_ph value %f to nvs", value);
        err = nvs_set_u32(my_handle, "min_ph", to_set);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
}

//todo add errors reporting here
float nvs_get_max_ph()
{
    uint32_t value;
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
        ESP_LOGI(NVS_TAG, "reading max_ph");
        err = nvs_get_u32(my_handle, "max_ph", &value);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
    return round(value / nvs_ph_multiplier);
}

//todo add errors reporting here
void nvs_set_max_ph(float value)
{
    uint32_t to_set = value * nvs_ph_multiplier;
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
        ESP_LOGI(NVS_TAG, "trying to write max_ph value %f to nvs", value);
        err = nvs_set_u32(my_handle, "max_ph", to_set);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
}

//todo add errors reporting here
bool nvs_get_ph4_mv(uint32_t *value)
{
    bool result = false;
        // Open
        ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }
    else
    {
        ESP_LOGI(NVS_TAG, "reading ph4_mv");
        err = nvs_get_u32(my_handle, "ph4_mv", value);
        result = hanlde_nvs_err(err);
        nvs_close(my_handle);
    }
    return result;
}

//todo add errors reporting here
void nvs_set_ph4_mv(uint32_t value)
{
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
        ESP_LOGI(NVS_TAG, "trying to write ph4_mv value %d to nvs", value);
        err = nvs_set_u32(my_handle, "ph4_mv", value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
}


//todo add errors reporting here
bool nvs_get_ph7_mv(uint32_t *value)
{
    bool result = false;
        // Open
        ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }
    else
    {
        ESP_LOGI(NVS_TAG, "reading ph7_mv");
        err = nvs_get_u32(my_handle, "ph7_mv", value);
        result = hanlde_nvs_err(err);
        nvs_close(my_handle);
    }
    return result;
}

//todo add errors reporting here
void nvs_set_ph7_mv(uint32_t value)
{
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
        ESP_LOGI(NVS_TAG, "trying to write ph7_mv value %d to nvs", value);
        err = nvs_set_u32(my_handle, "ph7_mv", value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        ESP_LOGI(NVS_TAG, "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
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

void nvs_set_mqtt_pass(char *pass)
{
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
        ESP_LOGI(NVS_TAG, "trying  mqtt pass value %s to nvs", pass);
        err = nvs_set_str(my_handle, "mqtt_pass", pass);
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

void nvs_get_mqtt_pass(char *pass)
{
    // Open
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(NVS_TAG, "reading mqtt pass value");
        size_t size;
        err = nvs_get_str(my_handle, "mqtt_pass", pass, &size);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
}

void nvs_set_mqtt_ip(char *ip)
{
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
        ESP_LOGI(NVS_TAG, "trying  mqtt ip value %s to nvs", ip);
        err = nvs_set_str(my_handle, "mqtt_ip", ip);
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

void nvs_get_mqtt_ip(char *ip)
{
    // Open
    ESP_LOGI(NVS_TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(NVS_TAG, "reading mqtt pass value");
        size_t size;
        err = nvs_get_str(my_handle, "mqtt_ip", ip, &size);
        hanlde_nvs_err(err);
        // Close
        nvs_close(my_handle);
    }
}
// todo fix readwrite