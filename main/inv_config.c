#include "inv_config.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"

void inv_config(void) {
  gpio_set_direction(Inv_En_Pin, GPIO_MODE_DEF_OUTPUT);
  gpio_set_level(Inv_En_Pin, 1);

  /*
   * Prepare and set configuration of timers
   * that will be used by LED Controller
   */
  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_DUTY_RES,
    .freq_hz = LEDC_FREQUENCY,
    .speed_mode = LEDC_MODE,
    .timer_num = LEDC_TIMER,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  // Set configuration of timer0 for LOW speed channels
  ledc_timer_config(&ledc_timer);

  // Channel 0 to Inverter PWM
  ledc_channel_config_t ledc_channel_0 = {
    .channel    = LEDC_CHANNEL_0,
    .duty       = 0,
    .gpio_num   = Inv_PWM_Pin,
    .speed_mode = LEDC_MODE,
    .intr_type  = LEDC_INTR_DISABLE,
    .hpoint     = 0,
    .timer_sel  = LEDC_TIMER
  };
  ledc_channel_config(&ledc_channel_0);
}
