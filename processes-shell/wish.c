#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h> 
#include <unistd.h>

#include "parser.h"

#define ARGUMENT_VECTOR_BUFFER_SIZE (512)
#define INPUT_BUFFER_SIZE (10)
#define INITIAL_PA_INPUT_SIZE (1024)


FILE* g_in_fd;
FILE* g_out_fd;
FILE* g_err_fd;

static char s_error_message[30] = "An error has occurred\n";
static int s_current_pa_input_buffer_index = 0;

typedef enum {
    COMMAND_TYPE_BUILT_IN,
    COMMAND_TYPE_CUSTOM
} command_type_t;

typedef struct {
    int child_pid; // -1이면 아직 시작 안한거
    char* pa_input; // 나중에 해제해야해..
    char* argv_buffer[ARGUMENT_VECTOR_BUFFER_SIZE + 1];
    char* output_redirection_file_name; // NULL이면 리디렉션 안하는거임 ㅇㅋ?
    command_type_t command_type;
} child_process_info_t;

// 설계하지마라
/*
    - app.h
    shell_init() -> fd init, mode init, argc check

    - reader.h in app.h
    size_t command_buffer_index = 0;
    child_process_info_t command_buffer[20] = { NULL, };
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
        free all resources.


*/

int main(int argc, char* argv[])
{
    g_in_fd = stdin;
    g_out_fd = stdout;
    g_err_fd = stderr;
    char* pa_input = malloc(INITIAL_PA_INPUT_SIZE * sizeof(char));
    size_t input_capacity = 0;
    bool interactive_mode = false;
    
    // if argc > 3 error, argc == 2 batch mode, argc == 1 interactive mode;
    if (argc == 1) {
        // interactive mode, do nothing. go to while loop?
        interactive_mode = true;
    } else if (argc == 2) {
        // batch mode, argv[1] should be input file, redirect g_in_fd with file?
        char* input_file_name = argv[1];
        assert (input_file_name != NULL);
        g_in_fd = fopen(input_file_name, "r"); // TODO : in fd가 stdin이 아닐때는 닫아줘야 할텐데?
        if (g_in_fd == NULL) {
            goto error;
        }
    } else {
        // invalid usage, print error message and exit
        goto error;
    }

    while(true) {
        if (interactive_mode) {
            printf("wish> ");
        }

        // if batch file has commands with new line, this could be multiple vusdf
        int input_size = getline(&pa_input, &input_capacity, g_in_fd); // TODO : &를 아래서 처리하기.
        if (input_size == -1) {
            // listen to EOF signal(ctrl + d)
            // if (exit_signal) exit_handler();, exit child process if there is and comeback to shell's new loop
            // TODO : if child process , exit child process only else, goto exit_shell;
            goto exit_shell;
        }

        char* pa_input_copy = pa_input;
        char* command = strsep(pa_input_copy, "&");
        while (command != NULL) {
            char* argv_buffer[ARGUMENT_VECTOR_BUFFER_SIZE + 1] = { NULL, };
            parser_status_t parser_state = parse_single_command(command, argv_buffer);
            if (parser_state == PARSER_STATUS_INVALID_INPUT) {
                goto error;
            } else if (parser_state == PARSER_STATUS_COMMAND_REMAINING) {
                // save argv to somewhere else
            } else {
                // completed, exit!
            }
            
            // TODO : argv_buffer확인해서 빌트인 커맨드 핸들링..
            command = strsep(pa_input_copy, "&");
        }





        // 이제 argv 버퍼에 커맨드들을 의미하는 char*들이 배열안에 잘 들어가 있을 것임.
            // argv[0]이 빌트인이면, 빌트인 핸들러로
                // TODO : path 면 path 관리해야됨
            // 아니라면, execv using path
            // 함수화만 잘하면.. 끝날듯합니다.

        // fork먼저 해야지
        // TODO : execv path argument에 "path/%s", argv[0] 이딴식으로 해야함
        char* path = "/bin/";
        char* program = argv_buffer[0];
        int command_length = strlen(path) + strlen(program);
        char* run_path = malloc(command_length * sizeof(char) + 1);
        sprintf(run_path, "%s%s", path, program);

        int child_pid = fork();
        if (child_pid == -1) {
            goto error;
        } else if (child_pid == 0) {
            // child process
            
            int no_return = execv(run_path, argv_buffer);
            printf("wtf\n");
            if (no_return == -1) {
                // child process failed;
                goto error;
            }
        }
        // parent process

        int status;
        pid_t exit_status = waitpid(child_pid, &status, WNOHANG); // non blocking
        // TODO : use while to listen EOF. if EOF is heard, break out of while loop.

        // TODO : 자식 프로세스 관련 struct만들어서, 배열에 넣고관리하자...


        // argument handling
        // int pids[], char* input, char* commands[][], int output_fd = STDOUT_FILENO;
        // parser.c, validator.c, redirector.c?
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



        // should free all resources at this point.
    }

error:
    fprintf(g_err_fd, "%s", s_error_message);
    exit(1);
exit_shell:
    if (interactive_mode == false) {
        if (g_in_fd != NULL && g_in_fd != stdin) {
            // close(g_in_fd); // how to handle this?? i am exiting anyway
        }
    }
    if (pa_input != NULL) {
        free(pa_input);
        pa_input = NULL;
    }
    return 0;
}