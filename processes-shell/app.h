#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdbool.h>

#include "libs.h"
#include "command_executor.h"


int shell_init(int argc, char* argv[]);

// TODO : path malloc제거하는 함수 추가

static const char* const s_error_message = "An error has occurred\n";

#endif /* APP_H */