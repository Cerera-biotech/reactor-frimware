#pragma once
#include "driver/ledc.h"
#include "pin_config.h"
#include "nvs.h"

#define LED_CONTROL_CH_NUM (10)
#define LED_CONTROL_FADE_TIME (3000)
#define LED_CONTROL_FREQUENCY (5000)

static const char LED_TAG[] = "led";

ledc_channel_config_t ledc_channel[LED_CONTROL_CH_NUM];

uint32_t led_get_duty_of_channel(uint8_t ch)
{
    return ledc_get_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
}

void set_duty_with_fade_for_channel(uint8_t ch, uint32_t target_duty)
{
    if (target_duty > 1023)
    {
        target_duty = 1023; // bacause the resolution of the timer is 8
    }
    ESP_LOGI(LED_TAG, "setting ch %d to %d", ch, target_duty);
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel, target_duty, LED_CONTROL_FADE_TIME);
    ledc_fade_start(ledc_channel[ch].speed_mode,
                    ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
}

void set_duty_with_fast_fade_blocking(uint8_t ch, uint32_t target_duty)
{
    if (target_duty > 1023)
    {
        target_duty = 1023; // bacause the resolution of the timer is 8
    }
    ESP_LOGI(LED_TAG, "fast fade setting ch %d to %d", ch, target_duty);
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel, target_duty, 500);
    ledc_fade_start(ledc_channel[ch].speed_mode,
                    ledc_channel[ch].channel, LEDC_FADE_WAIT_DONE);
}

void test_leds()
{
    for (uint8_t ch = 0; ch < LED_CONTROL_CH_NUM; ch++)
    {
        ESP_LOGI(LED_TAG, "testing %d cahnnel, duty 100%%", ch);
        set_duty_with_fast_fade_blocking(ch, 1023);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        set_duty_with_fast_fade_blocking(ch, 0);
        ESP_LOGI(LED_TAG, "testing %d cahnnel done", ch);
    }
}

void led_setup_channel(uint8_t chan, uint8_t gpio)
{
    if (chan > LED_CONTROL_CH_NUM)
        return;
    uint8_t group = (chan / 8), channel = (chan % 8), timer = (chan / 8);
    ledc_channel_config_t config = {.channel = channel,
                                    .duty = 0,
                                    .gpio_num = gpio,
                                    .speed_mode = group,
                                    .hpoint = 0,
                                    .timer_sel = timer};
    ledc_channel[chan] = config;
}

void led_setup_timer(ledc_timer_t timer, ledc_mode_t mode)
{
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = LED_CONTROL_FREQUENCY,     // frequency of PWM signal
        .speed_mode = mode,                   // timer mode
        .timer_num = timer,                   // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));
}

void led_init(void)
{
    led_setup_channel(0, GPIO_LED_CH0);
    led_setup_channel(1, GPIO_LED_CH1);
    led_setup_channel(2, GPIO_LED_CH2);
    led_setup_channel(3, GPIO_LED_CH3);
    led_setup_channel(4, GPIO_LED_CH4);
    led_setup_channel(5, GPIO_LED_CH5);
    led_setup_channel(6, GPIO_LED_CH6);
    led_setup_channel(7, GPIO_LED_CH7);
    led_setup_channel(8, GPIO_LED_CH8);
    led_setup_channel(9, GPIO_LED_CH9);

    led_setup_timer(LEDC_TIMER_0, LEDC_HIGH_SPEED_MODE);
    led_setup_timer(LEDC_TIMER_1, LEDC_LOW_SPEED_MODE);
    // led_setup_timer(LEDC_TIMER_2, LEDC_HIGH_SPEED_MODE);
    // led_setup_timer(LEDC_TIMER_3, LEDC_HIGH_SPEED_MODE);
    // led_setup_timer(LEDC_TIMER_0, LEDC_LOW_SPEED_MODE);

    // Set LED Controller with previously prepared configuration
    for (uint8_t ch = 0; ch < LED_CONTROL_CH_NUM; ch++)
    {
        ESP_LOGI(LED_TAG, "configuring %d cahnnel %d on gpio %d ledc: %d", ledc_channel[ch].speed_mode, ch, ledc_channel[ch].gpio_num, ledc_channel[ch].channel);
        ledc_channel_config(&ledc_channel[ch]);
        ESP_LOGI(LED_TAG, "done");
    }

    // Initialize fade service.
    ledc_fade_func_install(0);
}

void set_leds_from_nvs()
{
    // Set LED Controller with previously prepared configuration
    for (uint8_t ch = 0; ch < LED_CONTROL_CH_NUM; ch++)
    {
        uint32_t duty = nvs_get_ch_value(ch);
        ESP_LOGI(LED_TAG, "initial setting ch %d to %d", ch, duty);
        if (duty)
            set_duty_with_fade_for_channel(ch, duty);
    }
}
