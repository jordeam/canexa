#include <string.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

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
