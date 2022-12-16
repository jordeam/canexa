#ifndef _STRSS_H
#define _STRSS_H

#include <stdio.h>

#ifndef STRSIZE
#define STRSIZE 1024
#endif

char * chomp(char * s);
size_t strrspn(const char * s, const char * skipset);
int number_tokens(const char *s, int len);

/*
Remove the beginning and ending spaces ou non graphical characters, and multiple
graphical characters between worlds, letting only one non-grafical character,
e.g., '\0' so it is easy to get the tokens. Returns string size to be used with
get_token and number_token.
*/
int split_tokens(char *s);
char *get_token(char *s, int len, int n);

#endif
