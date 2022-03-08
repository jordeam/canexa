#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "driver/twai.h"

#define TAG "main"

static const twai_message_t start_message = {
  .identifier = 0b00110111101110111101010111011,
  .data_length_code = 4,
  .extd = 1,
  .data = {0x01, 0x02 , 0x03 , 0x04 ,0x05 ,0x06 ,0x07 ,0x08}};

void twai_config(void) {
  //Initialize configuration structures using macro initializers
  /*!< new port V1.0 ============CAN=================era 16 e 33========================================*/
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CONFIG_TX_GPIO_NUM, (gpio_num_t)CONFIG_RX_GPIO_NUM, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  ESP_LOGI(TAG, "TX_PIN=%d RX_PIN=%d", CONFIG_TX_GPIO_NUM, CONFIG_RX_GPIO_NUM);

  //Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    ESP_LOGI(TAG, "TWAI Driver installed\n");
  } else {
    printf("Failed to install TWAI driver\n");
    return;
  }

  //Start TWAI driver
  if (twai_start() == ESP_OK) {
    ESP_LOGI(TAG, "TWAI Driver started");
  } else {
    ESP_LOGI(TAG, "Failed to start TWAI driver");
    return;
  }
}

void wait_twai_msg(void *pvParameters) {
  while (true) {
    twai_message_t msg;

    ESP_LOGV(TAG, "RAM left %d", esp_get_free_heap_size());
    ESP_LOGV(TAG, "wait_twai_msg task stack: %d", uxTaskGetStackHighWaterMark(NULL));

    /* Wait for message to be received */
    ESP_LOGD(TAG, "TWAI waiting for message...");
    if (twai_receive(&msg, pdMS_TO_TICKS(10000)) == ESP_OK) {
      ESP_LOGV(TAG, "Message received");
    } else
      continue;

    //Process received message
    if ((msg.flags & TWAI_MSG_FLAG_RTR)) {
      ESP_LOGD(TAG, "RTR ID=0x%08x %s", msg.identifier, (msg.flags & TWAI_MSG_FLAG_EXTD) ? "EXT" : "STD");
    }
    else {
      ESP_LOGD(TAG, "RTR ID=0x%08x %s size:%d", msg.identifier, (msg.flags & TWAI_MSG_FLAG_EXTD) ? "EXT" : "STD", msg.data_length_code);
      if (msg.identifier == 0x030001) {
        float temp;
        ((char *)&temp)[3] = msg.data[0];
        ((char *)&temp)[2] = msg.data[1];
        ((char *)&temp)[1] = msg.data[2];
        ((char *)&temp)[0] = msg.data[3];
        ESP_LOGD(TAG, "Temp = %f", temp);
      }
    }
    ESP_LOG_BUFFER_HEX("TWAI-main", msg.data, msg.data_length_code);
  }
  vTaskDelete(NULL);
}

static void twai_transmit_task(void *arg)
{
    while (true) {
        sleep(10);
        //Transmit start command to slave
        if (twai_transmit(&start_message, pdMS_TO_TICKS(100)) == ESP_OK)
          ESP_LOGI(TAG, "Message ID=0x%08x transmitted", start_message.identifier);
        else{
            ESP_LOGE(TAG, "Error sending message ");
        }
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

void app_main(void) {
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  twai_config();
  xTaskCreate(&wait_twai_msg, "wait_twai_msg", 4096, NULL, 5, NULL);
  xTaskCreate(&twai_transmit_task, "twai_transmit_task", 4096, NULL, 5, NULL);
}

