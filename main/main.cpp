/*
 * main.c
 *
 *  Created on: March 7, 2022
 *      Author: jrm
 */

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <cmath>

#include <cstring>
#include <iostream>
#include <cstdio>
#include <array>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
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

#include "commands.h"
#include "interpret_cmd.h"

#define TAG "main"

#define CMDSIZ 100

char cmd_line[CMDSIZ], read_buf[CMDSIZ];

/* static const twai_message_t start_message = { */
/*   .identifier = 0b00110111101110111101010111011, */
/*   .data_length_code = 4, */
/*   .extd = 1, */
/*   .data = {0x01, 0x02 , 0x03 , 0x04 ,0x05 ,0x06 ,0x07 ,0x08}}; */


void twai_msg_transmit(twai_message_t msg) {
  if (twai_transmit(&msg, pdMS_TO_TICKS(100)) == ESP_OK) {
    ESP_LOGI(TAG, "twai_transmit: Message ID=0x%08x transmitted",
             msg.identifier);
    printf("Data: ");
    for (int i = 0; i < msg.data_length_code; i++)
      printf("%02x", msg.data[i]);
    printf(" ");
    for (int i = 0; i < msg.data_length_code; i++)
      printf("%d ", msg.data[i]);
    printf("\n");
  } else {
    ESP_LOGE(TAG, "Error sending message ");
  }
}

static void twai_transmit_task(void *arg)
{
  twai_message_t msg;

  while (true) {
        sleep(5);
        msg.identifier = 0x2000106; //!< target power factor
        float fp = 0.95f;
        *((int32_t *)msg.data) = *((int32_t *)&fp);
        msg.data_length_code = 4;
        msg.extd = 1;
        msg.rtr = 0;
        twai_msg_transmit(msg);
        sleep(2);
        // Data request: parameters group 1.1
        msg.identifier = 0x200010b;
        msg.data_length_code = 1;
        msg.extd = 1;
        msg.rtr = 0;
        msg.data[0] = 1;
        twai_msg_transmit(msg);
        sleep(3);
        msg.identifier = 0x2000107;
        msg.rtr = 0;
        msg.data_length_code = 4;
        fp = 2e3;
        *((int32_t *)msg.data) = *((int32_t *)&fp);
        twai_msg_transmit(msg);
  }
  vTaskDelete(NULL);
}

void twai_delete(void) {
  //Stop the TWAI driver
  if (twai_stop() == ESP_OK) {
    ESP_LOGI(TAG, "Driver stopped");
  } else {
    ESP_LOGI(TAG, "Failed to stop driver");
    return;
  }

  //Uninstall the TWAI driver
  if (twai_driver_uninstall() == ESP_OK) {
    ESP_LOGI(TAG, "Driver uninstalled");
  } else {
    ESP_LOGI(TAG, "Failed to uninstall driver");
    return;
  }
}

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

float gsc_vbus_max = 740.0f;
float gsc_vbus_target_max = 700.0f;
float gsc_vbus_target_min = 680.0f;
float gsc_vbus_op_min = 660.0f;
float gsc_vbus_crit_min = 630.0f;

enum data_types { TPBIT, TPFLOAT, TPINT };

union data_types_u {
  float f;
  uint32_t d;
  bool b;
};

class twai_receive_table {
public:
  const char *nm;
  const data_types type; //< storage type for internal data
  data_types_u data;      //< 32 bits for data
  const uint32_t can_id;
  int pos;  //< position inside CAN message in bytes
  int size; //< occupied size in can message in bytes (0 if msg_type is float)
  int exp;  //< power 10 multiplier
};

std::array<twai_receive_table, 12> tab_msgs{
    {{"gsc_vbus", TPFLOAT, {.f = 400.0f}, 0x0100101, 0, 2, -1},
     {"gsc_vbus_peak", TPFLOAT, {.f = 800.0f}, 0x100010d, 4, 2, 0},
     {"gsc_power", TPFLOAT, {.f = 220e3f}, 0x0100101, 2, 2, 1},
     {"gsc_power_nom", TPFLOAT, {.f = 250e3f}, 0x100010d, 0, 2, 1},
     {"gsc_reactive_power", TPFLOAT, {.f = 50e3f}, 0x100010e, 0, 2, 1},
     {"gsc_vgrid_nom", TPFLOAT, {.f = 380.0f}, 0x100010c, 2, 2, -1},
     {"gsc_vgrid", TPFLOAT, {.f = 372.0f}, 0x0100101, 4, 2, -1},
     {"gsc_voltage_imbalance", TPFLOAT, {.f = 0.02f}, 0x100010e, 2, 2, -3},
     {"gsc_i_max_p", TPFLOAT, {.f = 510.0f}, 0x100010c, 4, 2, -1},
     {"gsc_i_line", TPFLOAT, {.f = 210.0f}, 0x100010e, 4, 2, -1},
     {"gsc_status", TPINT, {.d = 0}, 0x0100101, 6, 2, 0},
     {"gsc_hs_temp", TPFLOAT, {.f = 80.0f}, 0x0800105, 0, 0, 0}
    }};

static void uart_transmit_command_task(void *arg) {
  while (true) {
    for (auto &item : tab_msgs) {
      switch (item.type) {
      case TPFLOAT:
        std::cout << item.nm << " " << item.data.f << std::endl;
        break;
      case TPBIT:
        std::cout << item.nm << " " << item.data.b << std::endl;
        break;
      default:
        std::cout << item.nm << " " << item.data.d << std::endl;
      }
      if (strcmp(item.nm, "gsc_power") == 0)
        item.data.f = 200e3f;
    }
    sleep(5);
  }
}

// TASK
void wait_twai_msg(void *pvParameters) {
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

    //Process received message
    if (msg.rtr) {
      ESP_LOGD(TAG, "RECV: RTR ID=0x%08x %s size:%d", msg.identifier, (msg.flags & TWAI_MSG_FLAG_EXTD) ? "EXT" : "STD", msg.data_length_code);
    }
    else {
      ESP_LOGD(TAG, "RECV: ID=0x%08x %s size:%d", msg.identifier, (msg.flags & TWAI_MSG_FLAG_EXTD) ? "EXT" : "STD", msg.data_length_code);
      ESP_LOG_BUFFER_HEX("TWAI-main", msg.data, msg.data_length_code);
      for (auto &item : tab_msgs) {
        if (msg.identifier == item.can_id) {
          if (item.size == 0 && item.type == TPFLOAT) {
            // data in message is float, correcting endianess, because TMS320
            // is awckwardly big endian, big stupid architecure
            ((char *)&item.data.f)[3] = msg.data[0];
            ((char *)&item.data.f)[2] = msg.data[1];
            ((char *)&item.data.f)[1] = msg.data[2];
            ((char *)&item.data.f)[0] = msg.data[3];
          }
          else if (item.type == TPFLOAT) {
            // data in message is int but internal storage is float
            int32_t d = 0;
            for (int i = 0; i < item.size; i++)
              ((char *)&d)[item.size - i - 1] = msg.data[i];
            item.data.f = *((float *) &d);
            // multiplier:
            item.data.f *= powf(10.0f, item.exp);
          }
          else if (item.type == TPINT) {
            // data in message is int and internal storage is too
            int32_t d = 0;
            for (int i = 0; i < item.size; i++)
              ((char *) &d)[item.size - i - 1] = msg.data[i];
            item.data.d = d;
          }
          else if (item.type == TPBIT) {
            // storage is a bit, item.size is a position inside a byte
            uint8_t mask = 1;
            for (int i = 0; i < item.size && item.size < 8; i++)
              mask <<= 1;
            item.data.b = mask & msg.data[item.pos];
          }
        }
      }
    }
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

extern "C" void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    twai_config();
    xTaskCreate(&wait_twai_msg, "wait_twai_msg", 4096, NULL, 5, NULL);
    xTaskCreate(&twai_transmit_task, "twai_transmit_task", 4096, NULL, 5, NULL);
    xTaskCreate(&uart_read_command_task, "uart_read_command_task", 4096, NULL, 5, NULL);
    xTaskCreate(&uart_transmit_command_task, "uart_transmit_command_task", 4096, NULL, 5, NULL);
}
