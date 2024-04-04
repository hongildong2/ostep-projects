#include "command_executor.h"

#define COMMAND_BUFFER_SIZE (50)

static command_info_t command_info_buffer[COMMAND_BUFFER_SIZE] = { 0, };
static size_t current_command_buffer_size = 0;

size_t get_current_command_buffer_size(void)
{
    return current_command_buffer_size;
}

command_info_t get_command_info_at(size_t index)
{
    assert(index >= 0 && index < current_command_buffer_size);
    return command_info_buffer[index];
}

bool add_command(const char* const command)
{
    // call parser
    command_info_t command_info = get_command_info_at(current_command_buffer_size - 1);
    // TODO : 이거 진짜 되는건지 모르겟음.. 정안되면 vs 에서 디버깅하기..
    parser_status_t parsed = parse_single_command(command, &(command_info.argv_buffer), &(command_info.output_redirection_file_name));
    // if (parsed == PARSER_STATUS_INVALID_INPUT) ....
}

void clear_command_buffer(void)
{
    for (size_t i = 0; i < current_command_buffer_size; ++i) {
        // TODO : free command resources (pa_input)
    }
}

bool is_commands_done(void) {
    // loop command buffer, check child process progress using waitpid(WNOHANG)
          // int status;
        //pid_t exit_status = waitpid(child_pid, &status, WNOHANG); // non blocking
    return false;
}

void run_commands(void) {
    char* path = "/bin/";
    char* program = "asd";
    int command_length = strlen(path) + strlen(program);
    char* run_path = malloc(command_length * sizeof(char) + 1);
    sprintf(run_path, "%s%s", path, program);

        int child_pid = fork();
        if (child_pid == -1) {
        } else if (child_pid == 0) {
            // child process
            
            int no_return = execv(run_path, "asd");
            printf("wtf\n");
            if (no_return == -1) {
                // child process failed;
            }
        }
    //run and update buffer
    // TODO : 빌트인 커맨드 보이면 실행하기
    // TODO : if file_name in command_info is not null, redirect
}