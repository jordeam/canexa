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

// clear spaces before and after text
char * strtrim(char * s) {
  int i, j = 0;
  int n = strlen(s);

  j = strspn(s, " \t\n\r");
  if (j) {
    n -= j;
    for (i = 0; i < n; i++)
      s[i] = s[j++];
    s[i] = '\0';
  }
  //  printf("  j=%d, strrspn=%d\n", j, strrspn(s, " \t\n\r"));
  i = strrspn(s, " \t\n\r");
  //  printf("s = [%s] i = %d\n", s, i);
  s[strlen(s) - i] = '\0';
  return s;
}

char * strsubst(char * s, char * pat, char * subs) {
  char * sp;
  char * ss;
  int len;

  sp = strstr(s, pat);
  while (sp) {
    ss = strdup(sp + strlen(pat));
    * sp = '\0';
    len = strlen(s) + strlen(subs);
    strcat(s, subs);
    strcat(s, ss);
    free(ss);
#ifdef DEBUG
    printf("s = %s, pat = %s, subs = %s, ss = %s\n", s, pat, subs, ss);
#endif
    if (len < strlen(s))
      sp = strstr(s+len, pat);
    else
      break;
  }
  return s;
}

char * strnsubst(char * s, char * pat, char * subs, int n) {
  char * sp;
  char * ss;
  int len;
  int i = 0;

  sp = strstr(s, pat);
  while (sp && (i < n || i < 0)) {
    ss = strdup(sp + strlen(pat));
    * sp = '\0';
    len = strlen(s) + strlen(subs);
    strcat(s, subs);
    strcat(s, ss);
    free(ss);
#ifdef DEBUG
    printf("s = %s, pat = %s, subs = %s, ss = %s\n", s, pat, subs, ss);
#endif
    if (len < strlen(s))
      sp = strstr(s+len, pat);
    else
      break;
    i++;
  }
  return s;
}

/* char * newgetline(FILE * stream) { */
/*   char * s = NULL; */
/*   size_t len = 0; */
/*   getline(& s, & len, stream); */
/*   if (feof(stream)) */
/*     return NULL; */
/*   return s; */
/* } */

char * strcatdup(char ** ps, char * sa) {
  char * s = strdup(*ps);

  free(*ps);
  asprintf(ps, "%s%s", s, sa);
  free(s);

  return *ps;
}

/* char * strcasestr(char * str, char * substr) { */
/*   int i1, len1 = strlen(str); */
/*   int i2 = 0, len2 = strlen(substr); */

/*   for (i1 = 0; len2 && i1 <= len1 - len2; i1++) */
/*     if (tolower(str[i1]) == tolower(substr[i2])) { */
/*       i2++; */
/*       if (i2 >= len2) */
/* 	return (char *) (((int) str) + i1 - i2); */
/*     } */
/*   return NULL; */
/* } */

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

