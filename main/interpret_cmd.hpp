#pragma once

#include <stddef.h>
//#include <stdlib.h>

enum return_codes { not_found = 0, executed, executed_ok, not_executed, wrong_args, wrong_args_number };

typedef enum return_codes (*command_fcn)(char*,int,int);

struct command_entry {
  const char *cmd, *text;
  command_fcn fcn;
};

typedef struct command_entry command_entry_t;

extern const command_entry_t cmdtable[];

int interpret_cmd(char *s, size_t size);
