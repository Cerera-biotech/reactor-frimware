#pragma once

#include "driver/gpio.h"
#include "pin_config.h"

#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_MODE_BUTTON))

int read_mode_button()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    // xTaskCreate(button_reader, "button_reading_task", 2048, NULL, 10, NULL);
    return gpio_get_level(GPIO_MODE_BUTTON);
}