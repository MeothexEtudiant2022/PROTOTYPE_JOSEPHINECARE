
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define SPEAKER_GPIO 25
#define DUTY_RES LEDC_TIMER_13_BIT
#define DUTY_MAX ((1 << 13) - 1)
#define CHANNEL LEDC_CHANNEL_0

#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_F4  349
#define NOTE_F5  698

typedef struct {
    int frequency;
    int duration; // in ms
} note_t;

note_t star_wars_theme[] = {
    {NOTE_A4, 500}, {NOTE_A4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150},
    {NOTE_A4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150}, {NOTE_A4, 1000},
    {NOTE_E5, 500}, {NOTE_E5, 500}, {NOTE_E5, 500},
    {NOTE_F5, 350}, {NOTE_C5, 150}, {NOTE_GS4, 500}, {NOTE_F4, 350}, {NOTE_C5, 150}, {NOTE_A4, 1000},
    {0, 0}
};

void play_note(int freq, int duration_ms) {
    if (freq == 0) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, CHANNEL, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        return;
    }

    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, freq);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, CHANNEL, DUTY_MAX / 2);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, CHANNEL, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(50));
}

void app_main(void) {
    ledc_timer_config_t timer_conf = {
        .duty_resolution = DUTY_RES,
        .freq_hz = 1000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .channel    = CHANNEL,
        .duty       = 0,
        .gpio_num   = SPEAKER_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&channel_conf);

    for (int i = 0; star_wars_theme[i].duration != 0; i++) {
        play_note(star_wars_theme[i].frequency, star_wars_theme[i].duration);
    }
}
