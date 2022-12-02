#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>

#include "interpret_cmd.hpp"

#include "strss.h"

/*
 * Interpret commands.
 * It will destroy string s
 */
int interpret_cmd(char *s, size_t size) {
  command_entry_t *cmde;
  int s_orig_len, n_tokens;
  enum return_codes r = not_found;
  s_orig_len = split_tokens(s);
  n_tokens = number_tokens(s, s_orig_len);

  for (cmde = (command_entry_t *) cmdtable; cmde->cmd != NULL; cmde++) {
    if ((strlen(s) == 1 && s[0] == cmde->cmd[0]) || strcmp(cmde->cmd, s) == 0) {
      r = cmde->fcn(s, s_orig_len, n_tokens);
    }
  }

  switch (r) {
  case not_found:
    printf("ERROR: %s command not found\n", s);
    break;
  case not_executed:
    printf("ERROR: %s: command not executed\n", s);
    break;
  case wrong_args_number:
    printf("ERROR: %s: wrong number of arguments\n", s);
    break;
  case wrong_args:
    printf("ERROR: %s: wrong arguments\n", s);
    break;
  case executed_ok:
    /* printf("OK: %s\n", s); */
  default:
    break;
  }

  return (int) r;
}
