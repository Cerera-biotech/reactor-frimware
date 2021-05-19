#pragma once

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include <mbedtls/base64.h>

void get_device_id(char **device_id)
{
    uint8_t derived_mac_addr[6] = {0};
    //Get MAC address for WiFi Station interface
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA));
    asprintf(device_id, "%x%x%x%x%x%x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
}