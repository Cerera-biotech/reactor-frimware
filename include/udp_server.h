#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "jsmn.h"
#include <time.h>
#include "lwip/apps/sntp.h"
#include "driver/gpio.h"
#include "cJSON.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "config.h"

#include "esp_netif.h"
// #include "protocol_examples_common.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include "device_id.h"

const char *udpTAG = "UDP_SRV";

#define CONFIG_UDP_FULFILMENT_KEY "photoreactor-discovery"
#define CONFIG_UDP_FULFILMENT_PORT 2227
// #define CONFIG_UDP_WRITE_PORT 2228

//udp server that listens to udp identify requests
static void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1)
    {

        if (addr_family == AF_INET)
        {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(CONFIG_UDP_FULFILMENT_PORT);
            ip_protocol = IPPROTO_IP;
        }
        else if (addr_family == AF_INET6)
        {
            bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
            dest_addr.sin6_family = AF_INET6;
            dest_addr.sin6_port = htons(CONFIG_UDP_FULFILMENT_PORT);
            ip_protocol = IPPROTO_IPV6;
        }

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(udpTAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(udpTAG, "Socket created");

#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
        if (addr_family == AF_INET6)
        {
            // Note that by default IPV6 binds to both protocols, it is must be disabled
            // if both protocols used at the same time (used in CI)
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
        }
#endif

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(udpTAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(udpTAG, "Socket bound, port %d", CONFIG_UDP_FULFILMENT_PORT);

        while (1)
        {

            ESP_LOGI(udpTAG, "Waiting for data");
            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0)
            {
                ESP_LOGE(udpTAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else
            {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET)
                {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                }
                else if (source_addr.ss_family == PF_INET6)
                {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(udpTAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(udpTAG, "%s", rx_buffer);

                char *device_id_to_send;
                get_device_id(&device_id_to_send);
                // check if identify message has the right content
                if (strcmp(rx_buffer, CONFIG_UDP_FULFILMENT_KEY) == 0)
                {
                    ESP_LOGI(udpTAG, "disovery key is verified");
                    ESP_LOGI(udpTAG, "sending %s; len %d to %s:%d", device_id_to_send, strlen(device_id_to_send), addr_str, ((struct sockaddr_in *)&source_addr)->sin_port);
                    int err = sendto(sock, device_id_to_send, strlen(device_id_to_send), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0)
                    {
                        ESP_LOGE(udpTAG, "Error occurred during sending: errno %d", errno);
                        break;
                    }
                }
                else
                {
                    ESP_LOGI(udpTAG, "got non discovery command");
                    char *token = strtok(rx_buffer, "|");
                    char *directive = NULL;
                    char *device_id = NULL;
                    char *password = NULL;
                    uint i = 0;

                    // loop through the string to extract all other tokens
                    while (token != NULL)
                    {
                        ESP_LOGI(udpTAG, "part %d is `%s`\n", i, token); //printing each token
                        int len = strlen(token);
                        switch (i)
                        {
                        case 0:
                            // directive = strdup(token);
                            directive = malloc(len + 1);
                            strcpy(directive, token);
                            break;
                        case 1:
                            device_id = malloc(len + 1);
                            strcpy(device_id, token);
                            break;
                        case 2:
                            password = malloc(len + 1);
                            strcpy(password, token);
                            break;
                        default:
                            break;
                        }
                        i++;
                        token = strtok(NULL, " ");
                    }

                    if (strcmp(directive, "connect") == 0)
                    {

                        ESP_LOGI(udpTAG, "got the connect directive");
                        if (device_id == NULL)
                        {
                            ESP_LOGE(udpTAG, "no device id provided");
                            goto finish;
                        }
                        if (strcmp(device_id_to_send, device_id) != 0)
                        {
                            ESP_LOGE(udpTAG, "wrong device_id provided: `%s`; expected `%s`", device_id, device_id_to_send);
                            goto finish;
                        }
                        if (password != NULL)
                        {
                            ESP_LOGE(udpTAG, "no password provided");
                            goto finish;
                        }

                        ESP_LOGI(udpTAG, "directive verified");
                        ESP_LOGI(udpTAG, "IP: `%s`, password: `%s`", addr_str, password);
                        // todo store the password and ip of the server to nvs
                        // todo implement the connection process
                    }
                    else
                    {
                        ESP_LOGI(udpTAG, "got unsupported directive: %s", directive);
                    }
                finish:
                    free(directive);
                    free(device_id);
                    free(password);
                }
                free(device_id_to_send);
            }
        }

        if (sock != -1)
        {
            ESP_LOGE(udpTAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void udp_server_start()
{
    xTaskCreate(udp_server_task, "udp_server", 4096, (void *)AF_INET, 8, NULL);
    ESP_LOGI(udpTAG, "udp server started");
}