/*
 * main.c
 *
 *  Created on: March 7, 2022
 *      Author: jrm
 */

#include "fcntl.h"
#include "sys/types.h"
#include "sys/cdefs.h"
#include "esp_vfs_common.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"

#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
//#include "freertos/projdefs.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_types.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_console.h"
#include "hal/twai_types.h"
#include "esp_log.h"
#include "driver/ledc.h"

#include "driver/twai.h"

#define TAG "twai_cfg"

void twai_config(void) {
  // Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CONFIG_TX_GPIO_NUM, (gpio_num_t)CONFIG_RX_GPIO_NUM,
      TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  ESP_LOGI(TAG, "TX_PIN=%d RX_PIN=%d", CONFIG_TX_GPIO_NUM, CONFIG_RX_GPIO_NUM);
  esp_err_t err;
  // Install TWAI driver
  if ((err = twai_driver_install(&g_config, &t_config, &f_config)) == ESP_OK) {
//    ESP_LOGI(TAG, "TWAI Driver installed\n");
  } else {
    printf("ERROR: twai_driver_install error code 0x%x\n", err);
    return;
  }

  // Start TWAI driver
  if ((err = twai_start()) == ESP_OK) {
    ESP_LOGI(TAG, "TWAI Driver started OK");
  } else {
    ESP_LOGE(TAG, "twai_start error code 0x%x", err);
    return;
  }
}

void twai_delete(void) {
  esp_err_t err;
  // Stop the TWAI driver
  if ((err = twai_stop()) == ESP_OK) {
    ESP_LOGI(TAG, "Driver stopped OK");
  } else {
    ESP_LOGE(TAG, "twai_stop error code 0x%x", err);
    return;
  }
  // Uninstall the TWAI driver
  if (twai_driver_uninstall() == ESP_OK) {
    ESP_LOGI(TAG, "Driver uninstalled OK");
  } else {
    ESP_LOGE(TAG, "twai_driver_uninstall error code 0x%x", err);
    return;
  }
}

