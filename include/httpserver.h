#include <esp_http_server.h>
#include <sys/param.h>
#include "led.h"
#include "nvs.h"
#include "cJSON.h"

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
const static char http_200_hdr[] = "200 OK";
const static char http_content_type_html[] = "text/html";
const static char http_content_type_json[] = "application/json";

/* @brief tag used for ESP serial console messages */
static const char HTTP_TAG[] = "HTTP_SERVER";

// Create array
cJSON *Create_array_of_anything(cJSON **objects, int array_num)
{
    cJSON *prev = 0;
    cJSON *root;
    root = cJSON_CreateArray();
    for (int i = 0; i < array_num; i++)
    {
        if (!i)
        {
            root->child = objects[i];
        }
        else
        {
            prev->next = objects[i];
            objects[i]->prev = prev;
        }
        prev = objects[i];
    }
    return root;
}

/* Our URI handler function to be called during GET /uri request */
esp_err_t get_handler(httpd_req_t *req)
{
    cJSON *objects[LED_CONTROL_CH_NUM];
    for (int i = 0; i < LED_CONTROL_CH_NUM; i++)
    {
        objects[i] = cJSON_CreateObject();
    }

    cJSON *root;
    root = Create_array_of_anything(objects, LED_CONTROL_CH_NUM);
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    for (int i = 0; i < LED_CONTROL_CH_NUM; i++)
    {
        uint32_t duty = led_get_duty_of_channel(i);
        ESP_LOGI(HTTP_TAG, "ch %d duty %d", i, duty);
        cJSON_AddNumberToObject(objects[i], "duty", duty);
    }

    const char *my_json_string = cJSON_Print(root);
    // ESP_LOGI(HTTP_TAG, "my_json_string\n%s", my_json_string);
    cJSON_Delete(root);

    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_json);
    httpd_resp_send(req, my_json_string, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Our URI handler function to be called during GET /uri request */
esp_err_t get_root_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, http_200_hdr);
    httpd_resp_set_type(req, http_content_type_html);
    httpd_resp_send(req, (char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t put_handler(httpd_req_t *req)
{
    uint8_t channel;
    uint32_t duty;

    bool duty_found, channel_found = false;
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        char *buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            ESP_LOGI(HTTP_TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "channel", param, sizeof(param)) == ESP_OK)
            {
                ESP_LOGI(HTTP_TAG, "Found URL query parameter => channel=%s", param);
                channel = atoi(param);
                channel_found = true;
            }
            if (httpd_query_key_value(buf, "duty", param, sizeof(param)) == ESP_OK)
            {
                ESP_LOGI(HTTP_TAG, "Found URL query parameter => duty=%s", param);
                duty = atoi(param);
                duty_found = true;
            }
        }
        free(buf);
    }

    if (duty_found && channel_found)
    {
        nvs_set_ch_value(channel, duty);
        set_duty_with_fade_for_channel(channel, duty);
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "duty and channel parameters should exist");
        return ESP_OK;
    }
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri = "/channels",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

/* URI handler structure for GET /uri */
httpd_uri_t uri_get_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_root_handler,
    .user_ctx = NULL};

/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri = "/channels",
    .method = HTTP_PUT,
    .handler = put_handler,
    .user_ctx = NULL};

httpd_handle_t server;

/* Function for starting the webserver */
void start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
        httpd_register_uri_handler(server, &uri_get_root);
    }
}

/* Function for stopping the webserver */
void stop_webserver()
{
    if (server)
    {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}