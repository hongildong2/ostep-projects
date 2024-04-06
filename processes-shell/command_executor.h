#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


#include "libs.h"
#include "parser.h"

// 나도 이러고싶지 않았어요


size_t get_current_command_buffer_size(void);

command_info_t* get_command_info_at(size_t index);

bool add_command(char* const command_string);

void clear_command_buffer(void);

void run_commands(void);

bool is_commands_done(void);

void kill_all();

#endif /* COMMAND_EXECUTOR_H */