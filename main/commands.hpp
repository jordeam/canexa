#pragma once

#include "interpret_cmd.hpp"

#define SOFTWARE_ID "GMSC_V1"

enum return_codes cmd_version(char *, int, int);
enum return_codes cmd_twai_send(char *, int, int);
enum return_codes cmd_list_ids(char *, int, int);
enum return_codes cmd_list_cmds(char *, int, int);
enum return_codes cmd_twai(char *, int, int);
enum return_codes cmd_inv(char *, int, int);
enum return_codes cmd_led(char *, int, int);
