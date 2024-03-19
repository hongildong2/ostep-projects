#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> 

int main(int argc, char* argv[])
{
    // int pids[], char* input, char* commands[][], int output_fd = STDOUT_FILENO;
    // parser.c, validator.c, redirector.c?
    // if argc > 3 error, argc == 2 batch mode, argc == 1 interactive mode;

    // free all allocated memories by getline() and parse_malloc() before new loop;
    // read input using getline
    // parse input and to some 2D array (may be multiple commands)
        // parse
    // if command is built-in, validate it
    // if not built-in, try attching element of path array. validate first element using access()
        // check if there is redirection specifier >, if there is...
            // redirect stdout, stderr to specified file
            // make '>' NULL to make arguments to be NULL-terminated.
    // fork and call execv
    // waitpid with WNOHANG, check if EOF is entered and call exit(1) if.
    return 0;
}