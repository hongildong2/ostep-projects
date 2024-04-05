#include "command_executor.h"

#define COMMAND_BUFFER_SIZE (50)

static command_info_t command_info_buffer[COMMAND_BUFFER_SIZE] = { 0, };
static size_t current_command_buffer_size = 0;

void run_builtin_command(char** argv);
bool is_builtin_command(char* program_name);
char* get_program_path_malloc(char* program_name);

size_t get_current_command_buffer_size(void)
{
    return current_command_buffer_size;
}

command_info_t* get_command_info_at(size_t index)
{
    assert(index >= 0 && index < current_command_buffer_size);
    return command_info_buffer + index;
}

bool add_command(const char* const command_string)
{
    command_info_t* command_info_ptr = get_command_info_at(current_command_buffer_size++);
    parser_status_t parsed = parse_single_command(command_string, command_info_ptr);
    if (parsed == PARSER_STATUS_INVALID_INPUT) {
        --current_command_buffer_size;
        return false;
    }
    // not ran yet
    command_info_ptr->pa_input = command_string;
    command_info_ptr->pid = -1;
    command_info_ptr->status = -1;

    char* program_name = command_info_ptr->argv_buffer[0];
    if (is_builtin_command(program_name)) {
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
        char* alloced_input = command_ptr->pa_input;
        command_ptr->pid = -1;
        command_ptr->status = -1;
        if(alloced_input != NULL) {
            free(alloced_input);
            alloced_input = NULL;
        }
    }

    current_command_buffer_size = 0;
}

bool is_commands_done(void) {
    // loop command buffer, check child process progress using waitpid(WNOHANG)
          // int status;
        //pid_t exit_status = waitpid(child_pid, &status, WNOHANG); // non blocking

    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        command_info_t* command_ptr = get_command_info_at(i);
        int child_pid = command_ptr->pid;
        int status;
        
        if (command_ptr->command_type == COMMAND_TYPE_BUILT_IN) {
            // no need to check built in command
            continue;
        }

        pid_t result = waitpid(child_pid, &status, WNOHANG);
        if (result == 0) {
            // still runnig
            return false;
        } 
    }

    return true;
}

void run_commands(void) {
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        command_info_t* command_ptr = get_command_info_at(i);
        if (command_ptr->command_type == COMMAND_TYPE_CUSTOM) {
            // check if it is valid program using access
            char* pa_program_path = get_program_path_malloc(command_ptr->argv_buffer[0]);
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
                    // TODO : redirect output
                }
                int no_return = execv(pa_program_path, command_ptr->argv_buffer);
                // not reachable
            } else {
                //parent process
                free(pa_program_path);
                pa_program_path = NULL;
                command_ptr->pid = child_pid;
                int status;
                command_ptr->status = (child_pid, &status, WNOHANG);
                // if status is 0, it means still running
                // if successful exit, child pid is returned
                // if error, -1 is returned
            }
        } else {
            // built in command
            // path, cd, exit
            run_builtin_command(command_ptr->argv_buffer);
        }
    }
}

bool is_builtin_command(char* program_name){
    if (strcmp(program_name, "exit") || strcmp(program_name, "cd") || strcmp(program_name, "path")) {
        return true;
    }
    return false;
}

void run_builtin_command(char** argv)
{
    char* program_name = argv[0];
    if (strcmp(program_name, "exit")) {
        // just exit?
        exit(0);
    } else if (strcmp(program_name, "cd")) {
        // TODO : call syscall
    } else if (strcmp(program_name, "path")) {
        set_path(argv[1]); // 여기부터 보내주면 되겠지? ㅎㅎ
    } else {
        assert (false);
    }
}

char* get_program_path_malloc(char* program_name)
{
    const char** const paths = get_paths();
    char* p_program_name = program_name;
    char** p_paths = paths;
    char* pa_path_buffer = malloc(1025 * sizeof(char));
    size_t buffer_index = 0;
    while (*p_paths != NULL) {
        char* path_name = *p_paths;
        // ㅈㄴ귀찮아
        if (strlen(path_name) + strlen(program_name) > 1024) {
            // 넌 그냥 나가
            goto invalid;
        }

        snprintf(pa_path_buffer, 1024, "%s/%s", path_name, p_program_name);

        printf("merges path is %s\n", pa_path_buffer);
        
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
    return pa_path_buffer;
}