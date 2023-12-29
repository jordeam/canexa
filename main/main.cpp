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

#include "commands.hpp"
#include "interpret_cmd.hpp"
#include "twai_msg_pool.hpp"

#include "../../esp-jrm-cxx/include/gpio_cxx.hpp"

extern "C" {
#include "inv_config.h"
}

#define TAG "main"

#define CMDSIZ 100

char cmd_line[CMDSIZ], read_buf[CMDSIZ];

using std::cout;

// static void read_command_task(void *arg) {
static void read_command() {
  ESP_LOGI(TAG, "Entering command_reader...");
  char c;
  // int c;
  static int i = 0;
  for (;;) {
    int len = uart_read_bytes(UART_NUM_0, &c, 1, portMAX_DELAY); // illegal instruction
    // std::cin.get(&c, 1); // it does not work oy any other overloads
    // c = fgetc(stdin);
    // std::cout << "got " << c << '\n';
    if (len <= 0) {
      cout << "WARN: len < 0\n";
      vTaskDelay(pdMS_TO_TICKS(100));
    }
    else {
      if (c >= ' ' && i < CMDSIZ - 1) {
        read_buf[i++] = c;
        // std::cout << "read_command: c=" << (char) c << '\n'; // " len=" << len << '\n';
      }
      else if (c == '\r' || c == '\n') {
        // std::cout << '\r' << '\n' << std::endl;
        read_buf[i] = '\0';
        if (i >= 1) {
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

//    ESP_LOGI(TAG, "RAM left %ld", esp_get_free_heap_size());
 //   ESP_LOGI(TAG, "wait_twai_msg task stack: %ld", uxTaskGetStackHighWaterMark(NULL));

    /* Wait for message to be received */
    std::cout << "INFO: TWAI waiting for message..." << std::endl;
    msg.data_length_code = 0;
    if (twai_receive(&msg, portMAX_DELAY) == ESP_OK) {
      std::cout << "INFO: Message received" << std::endl;
    } else
      continue;

    std::cout << "twai ";

    if (msg.flags && TWAI_MSG_FLAG_EXTD)
      std::cout << std::hex << std::setw(8) << std::setfill('0') << msg.identifier << ' ';
    else
      std::cout << std::hex << std::setw(4) << std::setfill('0') << msg.identifier << ' ';

    for (int i = 0; i < msg.data_length_code; i++) {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int) msg.data[i];
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
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CONFIG_TX_GPIO_NUM, (gpio_num_t)CONFIG_RX_GPIO_NUM,
      TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  ESP_LOGI(TAG, "TX_PIN=%d RX_PIN=%d", CONFIG_TX_GPIO_NUM, CONFIG_RX_GPIO_NUM);

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
//    ESP_LOGI(TAG, "TWAI Driver installed\n");
  } else {
    printf("Failed to install TWAI driver\n");
    return;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) {
//    ESP_LOGI(TAG, "TWAI Driver started");
  } else {
//    ESP_LOGI(TAG, "Failed to start TWAI driver");
    return;
  }
}

void twai_delete(void) {
  // Stop the TWAI driver
  if (twai_stop() == ESP_OK) {
//    ESP_LOGI(TAG, "Driver stopped");
  } else {
//    ESP_LOGI(TAG, "Failed to stop driver");
    return;
  }
  // Uninstall the TWAI driver
  if (twai_driver_uninstall() == ESP_OK) {
//    ESP_LOGI(TAG, "Driver uninstalled");
  } else {
//    ESP_LOGI(TAG, "Failed to uninstall driver");
    return;
  }
}


extern "C" void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_INFO);

    esp_console_dev_uart_config_t dev_uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    fflush(stdout);
    fsync(fileno(stdout));
    esp_console_dev_uart_config_t* dev_config = &dev_uart_config;
    esp_vfs_dev_uart_port_set_rx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(dev_config->channel, ESP_LINE_ENDINGS_CRLF);

        /* Configure UART. Note that REF_TICK/XTAL is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
#if SOC_UART_SUPPORT_REF_TICK
    uart_sclk_t clk_source = UART_SCLK_REF_TICK;
    // REF_TICK clock can't provide a high baudrate
    if (dev_config->baud_rate > 1 * 1000 * 1000) {
        clk_source = UART_SCLK_DEFAULT;
        ESP_LOGW(TAG, "light sleep UART wakeup might not work at the configured baud rate");
    }
#elif SOC_UART_SUPPORT_XTAL_CLK
    uart_sclk_t clk_source = UART_SCLK_XTAL;
#else
    #error "No UART clock source is aware of DFS"
#endif // SOC_UART_SUPPORT_xxx

    const uart_config_t uart_config = {
      .baud_rate = dev_config->baud_rate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .source_clk = clk_source,
    };
    uart_param_config(dev_config->channel, &uart_config);
    uart_set_pin(dev_config->channel, dev_config->tx_gpio_num, dev_config->rx_gpio_num, -1, -1);
    /* Install UART driver for interrupt-driven reads and writes */
    uart_driver_install(dev_config->channel, 256, 0, 0, NULL, 0);
/* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(dev_config->channel);

    inv_config();
    twai_config();
    xTaskCreate(&twai_receive_task, "wait_twai_msg", 4096, NULL, 5, NULL);
    read_command();
}
