#ifndef PARSER_H
#define PARSER_H

#define ARGUMENT_VECTOR_BUFFER_SIZE (512)

typedef enum {
    PARSER_STATUS_INVALID_INPUT = -1,
    PARSER_STATUS_COMMAND_REMAINING = 0,
    PARSER_STATUS_PARSE_COMPLETE = 1
} parser_status_t;


int parse_single_command(char* const pa_input, char** argv_out);

#endif /* PARSER_H */