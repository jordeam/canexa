#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "strss.h"

char * chomp(char * s) {
  int i;
  int n = strlen(s);

  i = strrspn(s, " \t\n\r");
  s[n - i] = '\0';

  return s;
}

// returns the position of
size_t strrspn(const char * s, const char * skipset) {
  int i;

  for (i = strlen(s) - 1; i >= 0 && strchr(skipset, s[i]); i--);
  return strlen(s) - i -1;
}

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

