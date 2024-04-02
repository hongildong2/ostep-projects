#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h> 
#include <unistd.h>
#include "app.h"

#define ARGUMENT_VECTOR_BUFFER_SIZE (512)
#define INPUT_BUFFER_SIZE (10)
#define INITIAL_PA_INPUT_SIZE (1024)


FILE* g_in_fd;
FILE* g_out_fd;
FILE* g_err_fd;

static char s_error_message[30] = "An error has occurred\n";
static s_current_pa_input_buffer_index = 0;

typedef enum {
    COMMAND_TYPE_BUILT_IN,
    COMMAND_TYPE_CUSTOM
} command_type_t;

typedef struct {
    int child_pid; // -1이면 아직 시작 안한거
    char* pa_input;
    char* argv_buffer[ARGUMENT_VECTOR_BUFFER_SIZE];
    char* output_redirection_file_name; // NULL이면 리디렉션 안하는거임 ㅇㅋ?
    command_type_t command_type;
} child_process_info_t;

// 설계하지마라

int main(int argc, char* argv[])
{
    g_in_fd = stdin;
    g_out_fd = stdout;
    g_err_fd = stderr;
    char* pa_input_buffer[INPUT_BUFFER_SIZE] = { 0, }; // TODO : parallel 나중에 구현하자..

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
        int input_size = getdelim(&pa_input, &input_capacity, g_in_fd, "\n&"); // TODO : 커맨드 버퍼 여러개 만들어서 &도 처리하기 여기서도 반복문 돌려야할듯?
        if (input_size == -1) {
            // listen to EOF signal(ctrl + d)
            // if (exit_signal) exit_handler();, exit child process if there is and comeback to shell's new loop
            // TODO : if child process , exit child process only else, goto exit_shell;
            goto exit_shell;
        }

        // parse
            // parse pa_input and to some 2D array (may be multiple commands)
                // parse
                // if fail, clear resources and continue; loop
        size_t  argument_array_size = 0;
        char* argv_buffer[ARGUMENT_VECTOR_BUFFER_SIZE + 1]; // 내가 수동으로 포인터 옮겨가면서 스트링 포인터 담을거임
        
        char* p_input = pa_input; // 이 포인터를 옮겨가면서, 구분자 만나거나 널캐릭터로 바꾸고, 위의 스트링 포인터가 제대로 널캐릭터 종료가 될 . 수있도록 한다
        char* input_argument_token = p_input; // 이걸 배열에 담을거야

        bool save_token = false;
        size_t file_name_token_index = -1;//널캐릭터까지 읽을려고 사이즈 + 1 (포문 말하는거임 ㅎㅎ)
        for (size_t input_index = 0; input_index < input_size + 1; ++input_index) {
            if (*p_input == ' ' || *p_input == '\t' || *p_input == '&') {
                *p_input = '\0'; // 토큰 스트링이 널종료되어서 정상적으로 읽히도록.
                // 근데 만약에.. 다음에 구분자가 있거나 입력 종료면? 지금 하면 안된다...
                input_argument_token = p_input + 1; // 구분자를 만났으면, 다음단어의 시작은 이 포인터 다음이겠지?, p_input이 움직이면서 널종료 해줄거야
                save_token = true; // 이 플래그가 찍힌 다음에, 커맨드로 간주되는 캐릭터가 나오면 그때 저장하면 됨
            }
            else if (*p_input == '>') {
                *p_input = '\0'; // > 리디렉션을 받았으면, 커맨드에 남겨두지 않고 그냥 stdin stdout 리디렉션동작만 해주면된다.
                input_argument_token = p_input + 1;
                file_name_token_index = argument_array_size; // current argument vector buffer index, file name token will be placed in this index when save token tiggered
                save_token = true;
            }
            else if (*p_input == '\0' || *p_input == EOF) {
                break; // 다읽었다.
            }
            else {
                // 이상한 캐릭터가 아니고 커맨드로 간주될 캐릭터들
                if (save_token) {
                    argv_buffer[argument_array_size++] = input_argument_token;
                    save_token = false;
                }
            }

            ++p_input;
        }
        *p_input = '\0'; // 마지막에 읽은 단어가 터지지않도록..

        assert (argument_array_size <= ARGUMENT_VECTOR_BUFFER_SIZE);
        argv_buffer[argument_array_size++] = NULL; // should be NULL terminated;

        if (file_name_token_index != -1) {
            /*
            리디렉션 지시자를 읽어서 파일이름이 저장되었다는 뜻. 규약상 이 파일 이름이 아규먼트 벡터 버퍼의 마지막 (널 종료를 제외한) 요소가 되어야 한다.
            하지만, 아규먼트 벡터 버퍼의 크기가 이 인덱스보다 2 초과라면, 잘못된 입력이므로 에러메세지 출력 후 종료
            [..., 파일이름, NULL]로 아규먼트 벡터 버퍼가 정상적으로 저장되었다면, 파일 이름 요소를 그냥 NULL로 바꿔버리면 올바른 커맨드가 된다.
            -> [..., NULL, NULL]
            */
           if (argument_array_size > file_name_token_index + 2) {
            // wrong input,
            goto error;
           }
            argv_buffer[file_name_token_index] = NULL; // 파일이름은 내가 알아서 리디렉션 잘 해주고, 커맨드에서는 뺀다.
            --argument_array_size;
        }        
        // parse end
        for (size_t i = 0; i < argument_array_size - 1; ++i) {
            assert(*argv_buffer[i] != '\0' && *argv_buffer[i] != NULL);
        }

        // 이제 argv 버퍼에 커맨드들을 의미하는 char*들이 배열안에 잘 들어가 있을 것임.
            // argv[0]이 빌트인이면, 빌트인 핸들러로
                // TODO : path 면 path 관리해야됨
            // 아니라면, execv using path
            // 함수화만 잘하면.. 끝날듯합니다.
        
        int child_pid = execv("./", argv_buffer);
        if (child_pid == -1) {
            goto error;
        }

        int status;
        pid_t exit_status = waitpid(&child_pid, &status, WNOHANG); // non blocking
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