#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h> 
#include <unistd.h>

#include "app.h"
#include "command_executor.h"
#include "parser.h"
#include "reader.h"

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

        int read_result = read_commands();
        if (read_result == -1) {
            error_handler();
        } else if (read_result == 2) {
            // void \n input
            continue;
        }

        
        run_commands();        


        // if EOF came in, this while will be breaken out.
        while (is_commands_done() == false ) {
            // if (feof(stdin) != 0) {
            //     clearerr(stdin);
            //     kill_all();
            // }
        }

        if (g_in_fd != stdin) {
            break;
        }
        clear_command_buffer();
        clear_reader_buffer();
    }

    clear_command_buffer();
    clear_reader_buffer();
    return 0;
}