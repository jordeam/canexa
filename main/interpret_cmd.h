#ifndef _interpreter_h
#define _interpreter_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>

enum return_codes { not_found = 0, executed, executed_ok, not_executed, wrong_args, wrong_args_number };

typedef enum return_codes (*command_fcn)(char*,int,int);

struct command_entry {
  char *cmd, *text;
  command_fcn fcn;
};

typedef struct command_entry command_entry_t;

extern const command_entry_t cmdtable[];

int number_tokens(const char * s, int len);
char * strtrim2(char *s);
int split_tokens(char *s);
char * get_token(char * s, int len, int n);
int interpret_cmd(char *s, size_t size);

#ifdef __cplusplus
}
#endif

#endif
