#include "driver/ledc.h"

#include "pin_config.h"

#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_HIGH_SPEED_MODE

#define LEDC_LS_TIMER LEDC_TIMER_1
#define LEDC_LS_MODE LEDC_LOW_SPEED_MODE

#define LEDC_TEST_CH_NUM (10)
#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (3000)

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
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_1,
     .duty = 0,
     .gpio_num = GPIO_LED_CH1,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_2,
     .duty = 0,
     .gpio_num = GPIO_LED_CH2,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_3,
     .duty = 0,
     .gpio_num = GPIO_LED_CH3,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_4,
     .duty = 0,
     .gpio_num = GPIO_LED_CH4,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_5,
     .duty = 0,
     .gpio_num = GPIO_LED_CH5,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_6,
     .duty = 0,
     .gpio_num = GPIO_LED_CH6,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_7,
     .duty = 0,
     .gpio_num = GPIO_LED_CH7,
     .speed_mode = LEDC_HS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_HS_TIMER},
    {.channel = LEDC_CHANNEL_0,
     .duty = 0,
     .gpio_num = GPIO_LED_CH8,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER},
    {.channel = LEDC_CHANNEL_1,
     .duty = 0,
     .gpio_num = GPIO_LED_CH9,
     .speed_mode = LEDC_LS_MODE,
     .hpoint = 0,
     .timer_sel = LEDC_LS_TIMER}};

void led_init(void)
{
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_LS_MODE,           // timer mode
        .timer_num = LEDC_LS_TIMER,           // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&timer_config);

    timer_config.speed_mode = LEDC_HS_MODE;
    timer_config.timer_num = LEDC_HS_TIMER;
    ledc_timer_config(&timer_config);

    // Set LED Controller with previously prepared configuration
    for (uint8_t ch = 0; ch < LEDC_TEST_CH_NUM; ch++)
    {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Initialize fade service.
    ledc_fade_func_install(0);
}

void set_duty_with_fade_for_channel(uint8_t ch, uint32_t target_duty)
{
    ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                            ledc_channel[ch].channel, target_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[ch].speed_mode,
                    ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
}
