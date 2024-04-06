#include "command_executor.h"
#include "app.h"
#include "reader.h"

#define COMMAND_BUFFER_SIZE (50)
#define PATH_STRING_BUFFER_SIZE (2048)

static command_info_t command_info_buffer[COMMAND_BUFFER_SIZE] = { 0, };
static size_t current_command_buffer_size = 0;

// forward declaration
void error_handler();

// private function
void __run_builtin_command(const int argc, char** const argv);
bool __is_builtin_command(char* program_name);
char* __get_program_path_malloc(char* program_name);

size_t get_current_command_buffer_size(void)
{
    return current_command_buffer_size;
}

command_info_t* get_command_info_at(size_t index)
{
    assert(index >= 0 && index < current_command_buffer_size);
    return command_info_buffer + index;
}

bool add_command(char* const command_string)
{
    command_info_t* command_info_ptr = get_command_info_at(current_command_buffer_size++);
    parser_status_t parsed = parse_single_command(command_string, command_info_ptr);
    if (parsed == PARSER_STATUS_INVALID_INPUT) {
        --current_command_buffer_size;
        return false;
    }

    // not ran yet
    command_info_ptr->pid = -1;
    command_info_ptr->status = -1;

    char* program_name = command_info_ptr->argv_buffer[0];
    if (__is_builtin_command(program_name)) {
        command_info_ptr->command_type = COMMAND_TYPE_BUILT_IN;
    } else {
        command_info_ptr->command_type = COMMAND_TYPE_CUSTOM;
    }

    return true;
}

void clear_command_buffer(void)
{
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        command_info_t* command_ptr = get_command_info_at(i);
        command_ptr->pid = -1;
        command_ptr->status = -1;
    }

    current_command_buffer_size = 0;
}

bool is_commands_done(void) {
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        command_info_t* command_ptr = get_command_info_at(i);
        int child_pid = command_ptr->pid;
        int status;
        
        if (command_ptr->command_type == COMMAND_TYPE_BUILT_IN) {
            // no need to check built in command
            continue;
        }

        pid_t result = waitpid(child_pid, &status, WNOHANG);
        command_ptr->status = result;
        if (result == 0) {
            // still runnig
            return false;
        } 
    }
    return true;
}

void kill_all()
{
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        kill(command_info_buffer[i].pid, SIGINT);
    }
    current_command_buffer_size = 0;
}

void run_commands(void) {
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        command_info_t* command_ptr = get_command_info_at(i);
        if (command_ptr->command_type == COMMAND_TYPE_CUSTOM) {
            // check if it is valid program using access
            char* pa_program_path = __get_program_path_malloc(command_ptr->argv_buffer[0]);
            if (pa_program_path == NULL) {
                // error!
                error_handler();
            }
            // child process 실행시키고, 부모프로세스에서 pa_program_path 해제
            int child_pid = fork();
            if (child_pid == -1) {
                free(pa_program_path);
                pa_program_path = NULL;
                error_handler();
            } else if (child_pid == 0) {
                //child process
                if (command_ptr->output_redirection_file_name != NULL) {
                    FILE* fp = fopen(command_ptr->output_redirection_file_name, "w");
                    g_err_fd = fp;
                    g_out_fd = fp;
                }
                int no_return = execv(pa_program_path, command_ptr->argv_buffer);
                // not reachable
            } else {
                //parent process, record child process info
                command_ptr->pid = child_pid;
                int status;
                command_ptr->status = waitpid(child_pid, &status, WNOHANG);
                // if status is 0, it means still running
                // if successful exit, child pid is returned
                // if error, -1 is returned

                free(pa_program_path);
                pa_program_path = NULL;
            }
        } else {
            // built in command
            // path, cd, exit
            __run_builtin_command(command_ptr->argc, command_ptr->argv_buffer);
        }
    }
}

bool __is_builtin_command(char* program_name){
    if (strcmp(program_name, "exit") == 0 || strcmp(program_name, "cd") == 0 || strcmp(program_name, "path") == 0) {
        return true;
    }
    return false;
}

void __run_builtin_command(const int argc, char** const argv)
{
    char* program_name = argv[0];
    if (strcmp(program_name, "exit") == 0) {
        // just exit?
        clear_reader_buffer();
        clear_command_buffer();
        if (argc != 1) {
            error_handler();
        }
        exit(0);
    } else if (strcmp(program_name, "cd") == 0) {
        if (argc == 2 && chdir(argv[1]) == 0) {
            return; // success, return!
        }
        error_handler(); // invalid command or error occured
    } else if (strcmp(program_name, "path") == 0) {
        if (argc < 2) {
            error_handler();
        }
        set_path(argv + 1); // 여기부터 보내주면 되겠지? ㅎㅎ
    } else {
        error_handler();
    }
}

char* __get_program_path_malloc(char* program_name)
{
    char** paths = get_paths();
    char* p_program_name = program_name;
    char** p_paths = paths;
    char* pa_path_buffer = malloc( (PATH_STRING_BUFFER_SIZE + 1) * sizeof(char));
    size_t buffer_index = 0;
    while (*p_paths != NULL) {
        char* path_name = *p_paths;
        if (strlen(path_name) + strlen(program_name) > PATH_STRING_BUFFER_SIZE) {
            // 넌 그냥 나가
            goto invalid;
        }

        snprintf(pa_path_buffer, 1024, "%s/%s", path_name, p_program_name);

        
        
        if (access(pa_path_buffer, X_OK) == -1) {
            goto invalid;
        }


        buffer_index = 0;
        p_paths++;
    }

    return pa_path_buffer;

invalid:
    free(pa_path_buffer);
    pa_path_buffer = NULL;
    return NULL;
}