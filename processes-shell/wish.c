#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> 
#include <unistd.h>
#include "app.h"

FILE* g_in;
FILE* g_out;
FILE* g_err;
int main(int argc, char* argv[])
{
    g_in = stdin;
    g_out = stdout;
    g_err = stderr;

    while(true) {
        // listen to signal(ctrl + c)
        // if (exit_signal) exit_handler();, exit child process and comback to shell's new loop


        // free all allocated memories by getline() and parse_malloc() before new loop;
        // read input using getline
        // parse input and to some 2D array (may be multiple commands)
            // parse
            // if fail, clear resources and continue; loop

        // argument handling
        // int pids[], char* input, char* commands[][], int output_fd = STDOUT_FILENO;
        // parser.c, validator.c, redirector.c?
        // if argc > 3 error, argc == 2 batch mode, argc == 1 interactive mode;
        // command_struct command = parse_input()
        // command_struct contains redirection specifier, builtin or not and some other helpful informations

        // if command is built-in, validate it
            // if (command.type == BUILT_IN (enum))
        // if not built-in, try attching element of path array. validate first element using access()
            // else result = validate(command)

        // if command has redirection specifier, redirect_io(command);
        // check if there is redirection specifier >, if there is...
            // redirect stdout, stderr to specified file
            // make '>' NULL to make arguments to be NULL-terminated.

        // all validation and setup done, good to go
        // fork new process and call execv in that process.
        // parent process wait for child process to exit in while loop
            // do not use blocking since you should here exit signal. just loop it.
        // waitpid with WNOHANG, check if EOF is entered and call exit(1) if.




    }
    




    return 0;
}