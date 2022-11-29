/*
 * main.c
 *
 *  Created on: March 7, 2022
 *      Author: jrm
 */

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <cmath>

#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <array>
#include <vector>

#include "freertos/FreeRTOS.h"
//#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "driver/gpio.h"
#include "hal/twai_types.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "driver/twai.h"

#include "commands.hpp"
#include "interpret_cmd.hpp"
#include "twai_msg_pool.hpp"

#define TAG "main"

#define CMDSIZ 100

char cmd_line[CMDSIZ], read_buf[CMDSIZ];

static void uart_read_command_task(void *arg) {
  ESP_LOGI(TAG, "Entering command_reader...");
  int c;
  static int i = 0;
  for (;;) {
    c = fgetc(stdin);
    if (c < 0) {
      vTaskDelay(pdMS_TO_TICKS(500));
      /* printf("version 0.0.1.pre.alpha\n"); */
    } else {
      /* putchar(c); */
      if (c >= ' ' && i < CMDSIZ - 1) {
        read_buf[i++] = c;
      } else if (c == '\r' || c == '\n') {
        read_buf[i] = '\0';
        if (i > 1) {
          strcpy(cmd_line, read_buf);
          interpret_cmd(cmd_line, CMDSIZ);
        }
        i = 0;
      }
    }
  }
}

//
// TASK
// Receive TWAI messages
//
void twai_receive_task(void *pvParameters) {
  while (true) {
    twai_message_t msg;

    ESP_LOGV(TAG, "RAM left %d", esp_get_free_heap_size());
    ESP_LOGV(TAG, "wait_twai_msg task stack: %d", uxTaskGetStackHighWaterMark(NULL));

    /* Wait for message to be received */
    ESP_LOGD(TAG, "TWAI waiting for message...");
    msg.data_length_code = 0;
    if (twai_receive(&msg, pdMS_TO_TICKS(10000)) == ESP_OK) {
      ESP_LOGV(TAG, "Message received");
    } else
      continue;

    std::cout << std::hex << std::setfill('0');

    if (msg.flags && TWAI_MSG_FLAG_EXTD)
      std::cout << std::setw(4);
    else
      std::cout << std::setw(2);

    // print ID
    std::cout << msg.identifier;
    std::cout << std::setw(2);

    int len = msg.data_length_code;
    for (int i = 0; i < len; i++) {
      std::cout << ' ';
      std::cout << msg.data[i];
    }

    // Process received message
    if (msg.rtr) {
      std::cout << ' ' << "RTR";
    }
    std::cout << std::endl;
  }
  vTaskDelete(NULL);
}

void twai_config(void) {
  // Initialize configuration structures using macro initializers
  /*!< new port V1.0 ============CAN=================era 16 e
   * 33========================================*/
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CONFIG_TX_GPIO_NUM, (gpio_num_t)CONFIG_RX_GPIO_NUM,
      TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  ESP_LOGI(TAG, "TX_PIN=%d RX_PIN=%d", CONFIG_TX_GPIO_NUM, CONFIG_RX_GPIO_NUM);

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    ESP_LOGI(TAG, "TWAI Driver installed\n");
  } else {
    printf("Failed to install TWAI driver\n");
    return;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) {
    ESP_LOGI(TAG, "TWAI Driver started");
  } else {
    ESP_LOGI(TAG, "Failed to start TWAI driver");
    return;
  }
}

void twai_delete(void) {
  // Stop the TWAI driver
  if (twai_stop() == ESP_OK) {
    ESP_LOGI(TAG, "Driver stopped");
  } else {
    ESP_LOGI(TAG, "Failed to stop driver");
    return;
  }
  // Uninstall the TWAI driver
  if (twai_driver_uninstall() == ESP_OK) {
    ESP_LOGI(TAG, "Driver uninstalled");
  } else {
    ESP_LOGI(TAG, "Failed to uninstall driver");
    return;
  }
}

extern "C" void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_INFO);
    twai_config();
    xTaskCreate(&twai_receive_task, "wait_twai_msg", 4096, NULL, 5, NULL);
    xTaskCreate(&uart_read_command_task, "uart_read_command_task", 4096, NULL, 5, NULL);
}
