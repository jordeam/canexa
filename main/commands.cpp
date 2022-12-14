#include "hal/twai_types.h"
//#include "driver/twai.h"
#include "driver/twai.h"
#include "freertos/projdefs.h"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <cstdio>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOFTWARE_ID "GMSC_V1"

#include "pbit.h"

#include "commands.hpp"
#include "hal/twai_types.h"
#include "interpret_cmd.hpp"
#include "twai_msg_pool.hpp"

#include "strss.h"

// list of commands, first letter must be different to be used in short commands
const command_entry_t cmdtable[] = {
    {"send", "Send a TWAI message composed of ID DATA.", cmd_twai_send},
    {"version", "software version", cmd_version},
    {"list", "List all registered TWAI Ids", cmd_list_ids},
    {"help", "List all available commands", cmd_list_cmds},
    {"twai", "Show all TWAI message contents", cmd_twai},
    {nullptr, nullptr, nullptr}};

// Return hex token from 01234567890abcdef or ABCDEF
// Return 0 on error;
int hex(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  else if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  else if ('A' <= c && c <= 'F')
    return c - 'A' + 10;
  else
    return 0;
}

enum return_codes cmd_twai_send(char *s, int s_orig_len, int n_tokens) {
  uint32_t id;
  uint8_t data[8];

  if (n_tokens == 3) {
    char *so = get_token(s, s_orig_len, 2);
    int dlen = strlen(so);
    // std::cout << "so=" << so << "(" << dlen << ")" << std::endl;
    if (dlen % 2 != 0 || dlen > 16)
      return wrong_args;
    /* opmode_set(duty_cycle); */
    char *s_id = get_token(s, s_orig_len, 1);
    id = (uint32_t)strtoul(s_id, NULL, 16);
    for (int i = 0; 2 * i < dlen; i++) {
      char c1 = so[2 * i];
      char c2 = so[2 * i + 1];
      data[i] = (hex(c1) << 4) + hex(c2);
    }
    // mount msg and send it
    twai_message_t msg;
    msg.identifier = id;
    msg.rtr = 0;
    msg.extd = id > 0x7ff ? 1 : 0;
    msg.data_length_code = dlen >> 1;
    memcpy(msg.data, data, msg.data_length_code);

    twai_transmit(&msg, pdMS_TO_TICKS(200));
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_twai(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    for (twai_recv_msg *p = msg_recv_pool; p->id != 0; p++) {
      std::cout << "twai " << std::hex << p->id << " ";
      for (int i = 0; i < p->dlc; i++)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)p->data[i];
      std::cout << std::endl;
    }
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_version(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    printf("%s", get_token(s, s_orig_len, 0));
    printf(" %s\n", SOFTWARE_ID);
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_list_ids(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    for (twai_recv_msg *p = msg_recv_pool; p->id != 0; p++)
      std::cout << "twai_id " << std::hex << p->id << std::endl;
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_list_cmds(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    for (command_entry_t *p = (command_entry_t *) cmdtable; p->fcn != nullptr; p++)
      std::cout << "cmd " << p->cmd << " - " << p->text << std::endl;
    return executed_ok;
  }
  return wrong_args_number;
}

