#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h> 
#include <unistd.h>

#include "app.h"

FILE* g_in_fd;
FILE* g_out_fd;
FILE* g_err_fd;
bool g_interactive_mode;

// 설계하지마라
/*
    - app.h
    shell_init() -> fd init, mode init, argc check

    - reader.h in app.h
    size_t command_buffer_index = 0;
    command_info_t command_buffer[20] = { NULL, };
    read_commands(command_buffer, &command_buffer_index) -> getline ~ parse

    - command_executor.h in app.h
    for command : commands,
        if built in, run built in functions
        else fork and call execv
    
    - app.h
    wait_child_processes(command_buffer);
        waitpid(pid, WNOHANG);
        while(true) listen to EOF from stdin if interactive
        if child process is interrupted, return -1;

    - app.h
    free_resources(command_buffer);
        using pid, if child process is running, kill it
        free all resources in buffer (pa_input);


*/

int main(int argc, char* argv[])
{    
    if ((shell_init(argc, argv)) == -1) {
        error_handler();
    }

    while(true) {
        if (g_interactive_mode) {
            printf("wish> ");
        }

        if (read_commands() == -1) {
            error_handler();
        }
        // 이 위까진 테스트 완료..

        // run_commands();
        
        //while (is_commands_done() == false) {
            // listen to EOF input from stdin, then break;    
        // }

        // clear_command_buffer();
    }



    // clear resources
    if (g_interactive_mode == false) {
        if (g_in_fd != NULL && g_in_fd != stdin) {
            // close(g_in_fd); // how to handle this?? i am exiting anyway
        }
    }
    return 0;
}