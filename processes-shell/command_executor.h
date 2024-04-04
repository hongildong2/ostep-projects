#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include <stdbool.h>
#include <assert.h>

#include "libs.h"
#include "parser.h"

// 나도 이러고싶지 않았어요

size_t get_current_command_buffer_size(void);

command_info_t get_command_info_at(size_t index);

bool add_command(const char* const command);

void clear_command_buffer(void);

void run_commands(void);

bool is_commands_done(void);

#endif /* COMMAND_EXECUTOR_H */