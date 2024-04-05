#ifndef READER_H
#define READER_H

#include <stdlib.h>
#include <string.h>

#include "libs.h"
#include "parser.h"
#include "command_executor.h"

extern FILE* g_in_fd;
extern FILE* g_out_fd;
extern FILE* g_err_fd;
extern bool g_interactive_mode;

int read_commands();

#endif /* READER_H */