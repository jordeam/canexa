#ifndef _STRSS_H
#define _STRSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef STRSIZE
#define STRSIZE 1024
#endif

char * chomp(char * s);
size_t strrspn(const char * s, const char * skipset);
char * strtrim(char * s);
char * strsubst(char * s, char * pat, char * subs);
char * strnsubst(char * s, char * pat, char * subs, int n);
char * newgetline(FILE * stream);
char * strcatdup(char ** ps, char * sa);
/* char * strcasestr(char * str, char * substr); */
int number_tokens(const char *s, int len);
char *strtrim2(char *s);
int split_tokens(char *s);
char *get_token(char *s, int len, int n);

#ifdef __cplusplus
}
#endif

#endif
