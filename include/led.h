#pragma once
#include "driver/ledc.h"

#include "pin_config.h"
#include "nvs.h"

#define LEDC_TEST_CH_NUM (5)
#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (3000)

static const char LED_TAG[] = "led";

/*
     * Prepare individual configuration
     * for each channel of LED Controller
     * by selecting:
     * - controller's channel number
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
    {.channel = LEDC_CHANNEL_0,
     .duty = 0,
     .gpio_num = GPIO_LED_CH0,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_0},
    {.channel = LEDC_CHANNEL_1,
     .duty = 0,
     .gpio_num = GPIO_LED_CH1,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_0},
    {.channel = LEDC_CHANNEL_2,
     .duty = 0,
     .gpio_num = GPIO_LED_CH2,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_0},
    {.channel = LEDC_CHANNEL_3,
     .duty = 0,
     .gpio_num = GPIO_LED_CH3,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_0},
    {.channel = LEDC_CHANNEL_4,
     .duty = 0,
     .gpio_num = GPIO_LED_CH4,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_1},
    {.channel = LEDC_CHANNEL_5,
     .duty = 0,
     .gpio_num = GPIO_LED_CH5,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_1},
    {.channel = LEDC_CHANNEL_6,
     .duty = 0,
     .gpio_num = GPIO_LED_CH6,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_1},
    {.channel = LEDC_CHANNEL_7,
     .duty = 0,
     .gpio_num = GPIO_LED_CH7,
     .speed_mode = LEDC_HIGH_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_1},
    {.channel = LEDC_CHANNEL_0,
     .duty = 0,
     .gpio_num = GPIO_LED_CH8,
     .speed_mode = LEDC_LOW_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_2},
    {.channel = LEDC_CHANNEL_1,
     .duty = 0,
     .gpio_num = GPIO_LED_CH9,
     .speed_mode = LEDC_LOW_SPEED_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_TIMER_2}};

void set_duty_with_fade_for_channel(uint8_t ch, uint32_t target_duty)
{
    if (target_duty > 1023)
    {
        target_duty = 1023; // bacause the resolution of the timer is 8
    }
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel, target_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[ch].speed_mode,
                    ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
}

void led_init(void)
{
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&timer_config);

    timer_config.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_config.timer_num = LEDC_TIMER_1;
    // Set configuration of timer1 for high speed channels
    ledc_timer_config(&timer_config);

    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.timer_num = LEDC_TIMER_2;
    // Set configuration of timer2 for low speed channels
    ledc_timer_config(&timer_config);

    // Set LED Controller with previously prepared configuration
    for (uint8_t ch = 0; ch < LEDC_TEST_CH_NUM; ch++)
    {
        ESP_LOGI(LED_TAG, "configuring cahnnel %d on gpio %d", ch, ledc_channel[ch].gpio_num);
        ledc_channel_config(&ledc_channel[ch]);
        ESP_LOGI(LED_TAG, "done");
    }

    // Initialize fade service.
    ledc_fade_func_install(0);

    // Set LED Controller with previously prepared configuration
    for (uint8_t ch = 0; ch < LEDC_TEST_CH_NUM; ch++)
    {
        uint32_t duty = nvs_get_ch_value(ch);
        set_duty_with_fade_for_channel(ch, duty);
    }
}
