#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdint>

#include "freertos/FreeRTOS.h"

#include "esp_err.h"

#include "driver/twai.h"
#include "freertos/task.h"
#include "hal/twai_types.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "twai_msg_pool.hpp"

const char *TAG = "twai_msg_pool";

// using std::cout;

void twai_msg_transmit(twai_message_t msg) {
  int err;
  if ((err = twai_transmit(&msg, pdMS_TO_TICKS(100))) == ESP_OK) {
//    ESP_LOGI(TAG, "twai_transmit: Message ID=0x%08x transmitted", msg.identifier);
    printf("twai_msg_transmit: Data: ");
    for (int i = 0; i < msg.data_length_code; i++)
      printf("%02x", msg.data[i]);
    printf(" ");
    for (int i = 0; i < msg.data_length_code; i++)
      printf("%d ", msg.data[i]);
    printf("\n");
  } else {
    std::cout << "ERR: TWAI transmit error code 0x" << std::hex << err << '\n';
  }
}

uint32_t make_extid(uint16_t prio, uint8_t src_grp, uint8_t owner_dev, uint8_t cmd) {
  return ((uint32_t)prio << 18) | ((uint32_t)src_grp << 14) |
         ((uint32_t)owner_dev << 8) | ((uint32_t)cmd);
}

twai_recv_msg::twai_recv_msg(uint16_t prio, uint8_t src_grp, uint8_t owner_dev, uint8_t cmd, const char *_name)
    : name(_name) {
  id = make_extid(prio, src_grp, owner_dev, cmd);
}

twai_recv_msg msg_recv_pool[] = {
  twai_recv_msg((int)TWAIPRIO::CC, 0, GSC_OWNER, 1, "Vbus Po Vgrid gsc_status"),
  twai_recv_msg((int)TWAIPRIO::TS, 0, GSC_OWNER, 3, "Heatsink temp"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 12, "Params 1/1"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 13, "Params 1/2"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 14, "Meas 1/1"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 15, "Meas 2/1"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 16, "Meas 2/2"),
  twai_recv_msg((int)TWAIPRIO::SI, 0, GSC_OWNER, 17, "Meas 2/3"),
  twai_recv_msg((int)TWAIPRIO::SM, 0, GSC_OWNER, 18, "ADC A 1 2 3 4"),
  twai_recv_msg((int)TWAIPRIO::SM, 0, GSC_OWNER, 19, "ADC B 14 2 3 4"),
  twai_recv_msg((int)TWAIPRIO::SM, 0, GSC_OWNER, 20, "ADC C 14 2 3 4"),
  twai_recv_msg(0, nullptr)
};
