#ifndef _commands_h
#define _commands_h

#ifdef __cplusplus
extern "C" {
#endif

#include "interpret_cmd.h"

extern int8_t hall;
extern float speed;
extern float current;
extern float heat_temp;

enum return_codes cmd_speed(char *, int, int);
enum return_codes cmd_current(char *, int, int);
enum return_codes cmd_dc(char *, int, int);
enum return_codes cmd_hall(char *, int, int);
enum return_codes cmd_version(char *, int, int);
enum return_codes cmd_temp(char *, int, int);

#ifdef __cplusplus
}
#endif

#endif
