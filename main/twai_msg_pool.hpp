#pragma once

#include <cstdint>

#include "driver/twai.h"

const int MAXMSGS = 20;
const int TWAI_MAX_DATA = 8;

const int GSC_OWNER = 1;

class twai_recv_msg {
public:
  uint32_t id = 0;
  union {
    struct {
      uint8_t extd: 1;
    };
    uint8_t flags;
  };
  uint8_t dlc;
  uint8_t data[TWAI_MAX_DATA];
  const char * name;
  twai_recv_msg(uint32_t id_, const char *_name) : id(id_), name(_name) {}
  twai_recv_msg(uint16_t prio, uint8_t src_grp, uint8_t owner_dev, uint8_t cmd, const char *_name);
};

enum class TWAIPRIO {
  PA = 0b01110000000,
  SI = 0b01101000000,
  TS = 0b01100100000,
  SM = 0b01100010000,
  SH = 0b01100001000,
  FM = 0b01100000100,
  CS = 0b00111000010,
  CC = 0b00000000001
};

extern twai_recv_msg msg_recv_pool[];

void twai_msg_transmit(twai_message_t msg);
