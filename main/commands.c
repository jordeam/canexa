#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SOFTWARE_ID "GMSC_V1"

#include "pbit.h"

#include "commands.h"
#include "interpret_cmd.h"

const command_entry_t cmdtable[] = {
    {"dc", "return current duty cycle in %", cmd_dc},
    {"speed", "return current speed in rad/s", cmd_speed},
    {"current", "return current speed in rad/s", cmd_current},
    {"hall", "current ihall sensors state", cmd_hall},
    {"version-id", "software version", cmd_version},
    {"temp", "current heatsink temperature", cmd_temp},
    {NULL, NULL, NULL}};

float dc = 0;
int8_t hall;
float speed;
float current;
float heat_temp;

enum return_codes cmd_dc(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 2) {
    /* opmode_set(duty_cycle); */
    dc = atoi(get_token(s, s_orig_len, 1)) * 0.01f;
  }
  if (n_tokens == 1 || n_tokens == 2) {
    printf("dc %d\n", (int) (100 * dc));
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_speed(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 2) {
    /* opmode_set(speed); */
    speed = atoi(get_token(s, s_orig_len, 1)) * 1.0f;
  }
  if (n_tokens == 1 || n_tokens == 2) {
    printf("speed %d\n", (int) speed);
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_current(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 2) {
    /* opmode_set(current); */
    current = atoi(get_token(s, s_orig_len, 1)) * 1.0f;
  }
  if (n_tokens == 1 || n_tokens == 2) {
    printf("current %4.1f\n", (double) current);
    return executed_ok;
  }
  return wrong_args_number;
}

enum return_codes cmd_hall(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    printf("hall %d %d %d\n", (hall & BIT0) ? 1 : 0, (hall & BIT1) ? 1 : 0, (hall & BIT2) ? 1 : 0);
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

enum return_codes cmd_temp(char *s, int s_orig_len, int n_tokens) {
  if (n_tokens == 1) {
    printf("%s", get_token(s, s_orig_len, 0));
    printf(" %g\n", heat_temp);
    return executed_ok;
  }
  return wrong_args_number;
}
