#include "reader.h"

int read_commands()
{
    // 이거 이렇게 하는게 맞나? 잘 생각해봐 ㅅㅂ 스택의 배열 안의 구조체 참조하려면 어떻게해야하는지
    size_t input_capacity = (INITIAL_PA_INPUT_SIZE + 1) * sizeof(char);
    char* pa_input = malloc(input_capacity);
    if (pa_input == NULL) {
        goto read_fail;
    }

    int input_size = getline(&pa_input, &input_capacity, g_in_fd);
    if (input_size == -1) {
        goto read_fail;
    }

    char* pa_input_copy = pa_input;
    char* command = strsep(&pa_input_copy, "&");
    while (command != NULL) {
        if (add_command(command) == false) {
            goto read_fail;
        }
        command = strsep(&pa_input_copy, "&");
    }
    return 1;
    read_fail:
        if (pa_input != NULL) {
            free(pa_input);
            pa_input = NULL;
        }
        return -1;
}