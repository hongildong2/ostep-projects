#include "reader.h"
#include <stdio.h>
#define PA_READ_STRINGS_BUFFER_SIZE (256)

static char* pa_read_input_strings[PA_READ_STRINGS_BUFFER_SIZE] = { NULL, };
static size_t buffer_index = 0;

int read_commands()
{
    size_t input_capacity = (INITIAL_PA_INPUT_SIZE + 1) * sizeof(char);
    char* pa_input = malloc(input_capacity);
    if (pa_input == NULL) {
            goto read_fail;
    }
    int input_size = getline(&pa_input, &input_capacity, g_in_fd);

    
    while (input_size != -1 && buffer_index < PA_READ_STRINGS_BUFFER_SIZE) {
        if (input_size == 1) {
            if (g_in_fd == stdin) {
                free(pa_input);
                pa_input = NULL;
                return 2;
            } else {
                // file, should ignore newline character 
                while (getc(g_in_fd) != '\n') {
                    // loop
                }
            }
        }
        char* pa_input_copy = pa_input;
        char* command = strsep(&pa_input_copy, "&");


        while (command != NULL) {
            if (add_command(command) == false) {
                goto read_fail;
            }

            command = strsep(&pa_input_copy, "&");
        }

        // 버퍼에 등록
        pa_read_input_strings[buffer_index++] = pa_input;

        if (g_in_fd == stdin) {
            // 인터랙티브면 한번만 해라..
            break;
        }

        // 재할당하고 다음 루프
        pa_input = malloc((INITIAL_PA_INPUT_SIZE + 1) * sizeof(char));
        if (pa_input == NULL) {
            goto read_fail;
        }
        input_size = getline(&pa_input, &input_capacity, g_in_fd);
    }

    if (feof(g_in_fd) == 0) {
        printf("여기서 막히나요?\n");
        // not eof reached but terminated, error
        // 너무 힘들어요
        if (g_in_fd != stdin)
        {
            goto read_fail;
        }
        
    } else {
        // eof reached, normal termination
        clearerr(g_in_fd);
    }

    // normal return
    return 1;
    // abnormal return
read_fail:
    return -1;
}

void clear_reader_buffer()
{
    for (size_t i = 0; i < buffer_index; ++i) {
        if (pa_read_input_strings[i] != NULL) {
            free(pa_read_input_strings[i]);
            pa_read_input_strings[i] = NULL;
        }
    }
}