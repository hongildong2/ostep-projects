#ifndef PARSER_H
#define PARSER_H

#include <assert.h>

#include "libs.h"

int parse_single_command(char* const pa_input, command_info_t* command_info);

#endif /* PARSER_H */