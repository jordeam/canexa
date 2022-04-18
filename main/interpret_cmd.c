#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>

#include "interpret_cmd.h"

/*
  Returns the number of tokens in s separated by nongraphical character.
*/
int number_tokens(const char * s, int len) {
  int i, count = 0, was_graph = 0;
  for (i = 0; i < len; i++)
    if (isgraph(s[i])) {
      if (!was_graph)
        count++;
      was_graph = 1;
    }
    else was_graph = 0;
  return count;
}

/*
  Remove the beginning and ending spaces or non graphical characters.
*/
char * strtrim2(char *s) {
  int i, j, len;
  /* Let's trim the end */
  for (i = strlen(s) - 1; !isgraph(s[i]) && i >= 0; i--)
    s[i] = '\0';
  /* let's trim the beginning */
  len = strlen(s);
  for (i = 0; !isgraph(s[i]) && i < len; i++);
  for (j = 0; i < len; i++, j++)
    s[j] = s[i];
  s[j] = '\0';
  return s;
}

/*
  Remove the beginning and ending spaces ou non graphical characters, and multiple graphical characters between worlds, letting only one non-grafical character, e.g., '\0' so it is easy to get the tokens. Returns string size to be used with get_token and number_token.
*/
int split_tokens(char *s) {
  int i, j, len = strlen(s);
  /* let's trim the beginning */
  for (i = 0; !isgraph(s[i]) && i < len; i++);
  /* Let's split the string */
  for (j = 0; i < len; j++)
    if (isgraph(s[i]))
      s[j] = s[i++];
    else {
      s[j] = '\0';
      while(!isgraph(s[++i]) && i < len);
    }
  s[j] = '\0';
  /* Let's trim the end */
  for (i = j; !isgraph(s[i]) && i >= 0; i--)
    s[i] = '\0';
  return i + 1;
}

/*
  Returns the pointer to nth token.
*/
char * get_token(char * s, int len, int n) {
  int i, count = 0, was_graph = 0;
  if (n == count) return s;
  for (i = 0; i < len; i++)
    if (isgraph(s[i])) {
      if (!was_graph) {
        count++;
        if (n == count - 1)
          return s + i;
      }
      was_graph = 1;
    }
    else was_graph = 0;
  return NULL;
}

/*
 * Interpret commands
  It will destroy string s
 */
int interpret_cmd(char *s, size_t size) {
  command_entry_t *cmde;
  int s_orig_len, n_tokens;
  enum return_codes r = not_found;
  s_orig_len = split_tokens(s);
  n_tokens = number_tokens(s, s_orig_len);

  for (cmde = (command_entry_t *) cmdtable; cmde->cmd != NULL; cmde++) {
    if (strcmp(cmde->cmd, s) == 0) {
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
