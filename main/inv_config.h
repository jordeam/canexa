#pragma once

#include "driver/gpio.h"

#define Inv_En_Pin GPIO_NUM_33
#define Inv_PWM_Pin GPIO_NUM_5

#define LEDC_TIMER              LEDC_TIMER_1
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_MAX           ((1 << 13) -1) // Set duty to 99.9%.
#define LEDC_FREQUENCY          4000 // Frequency in Hertz. Set frequency at 4 kHz

void inv_config(void);
