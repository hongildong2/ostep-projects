#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdbool.h>

#include "libs.h"
#include "command_executor.h"
#include "reader.h"

extern FILE* g_in_fd;
extern FILE* g_out_fd;
extern FILE* g_err_fd;
extern bool g_interactive_mode;


int shell_init(int argc, char* argv[]);

void set_path(char** seperated_paths);

char** get_paths();

void error_handler();

#endif /* APP_H */