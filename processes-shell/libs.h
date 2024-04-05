#ifndef LIBS_H
#define LIBS_H

#include <stdio.h>
#include <stdbool.h>

#define ARGUMENT_VECTOR_BUFFER_SIZE (512)
#define INPUT_BUFFER_SIZE (10)
#define INITIAL_PA_INPUT_SIZE (1024)

typedef enum {
    PARSER_STATUS_INVALID_INPUT = -1,
    PARSER_STATUS_COMMAND_REMAINING = 0,
    PARSER_STATUS_PARSE_COMPLETE = 1
} parser_status_t;

typedef enum {
    COMMAND_TYPE_BUILT_IN,
    COMMAND_TYPE_CUSTOM
} command_type_t;

typedef struct {
    int pid;
    pid_t status;
    char* pa_input; // 나중에 해제해야해..
    char* argv_buffer[ARGUMENT_VECTOR_BUFFER_SIZE + 1];
    char* output_redirection_file_name; // if null, no redireciton, freed when pa_input is freed
    command_type_t command_type;
} command_info_t;


#endif /* LIBS_H */