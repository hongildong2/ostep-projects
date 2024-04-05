#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "parser.h"

int parse_single_command(const char* const pa_input, command_info_t* command_info_out)
{
    size_t argv_size = 0;
    char** argv_buffer = command_info_out->argv_buffer;
    
    char* p_input = pa_input; // 이 포인터를 옮겨가면서, 구분자 만나거나 널캐릭터로 바꾸고, 위의 스트링 포인터가 제대로 널캐릭터 종료가 될 . 수있도록 한다
    char* input_argument_token = p_input; // 이걸 배열에 담을거야

    bool save_token = true;
    size_t file_name_token_index = -1;
    while (*p_input != '\0' && *p_input != EOF && *p_input != '&') {
        if (*p_input == ' ' || *p_input == '\t' || *p_input == '\n') {
            *p_input = '\0'; // 토큰 스트링이 널종료되어서 정상적으로 읽히도록.
            // 근데 만약에.. 다음에 구분자가 있거나 입력 종료면? 지금 하면 안된다...
            input_argument_token = p_input + 1; // 구분자를 만났으면, 다음단어의 시작은 이 포인터 다음이겠지?, p_input이 움직이면서 널종료 해줄거야
            save_token = true; // 이 플래그가 찍힌 다음에, 커맨드로 간주되는 캐릭터가 나오면 그때 저장하면 됨
        }
        else if (*p_input == '>') {
            if (file_name_token_index != -1) {
                // 인풋파일이 여러개? ㄴㄴ 안됨
                return PARSER_STATUS_INVALID_INPUT;
            }
            *p_input = '\0'; // '>' 리디렉션을 받았으면, '>'를 커맨드에 남겨두지 않고 그냥 stdin stdout 리디렉션동작만 해주면된다.
            input_argument_token = p_input + 1;
            file_name_token_index = argv_size; // current argument vector buffer index, file name token will be placed in this index when save token tiggered
            save_token = true;
        }
        else {
            // 이상한 캐릭터가 아니고 커맨드로 간주될 캐릭터들
            if (save_token) {
                argv_buffer[argv_size++] = input_argument_token;
                save_token = false;
            }
        }

        ++p_input;
    }

    assert (argv_size <= ARGUMENT_VECTOR_BUFFER_SIZE);
    argv_buffer[argv_size++] = NULL; // should be NULL terminated;
     
    if (file_name_token_index != -1) {
        /*
        리디렉션 지시자를 읽어서 파일이름이 저장되었다는 뜻. 규약상 이 파일 이름이 아규먼트 벡터 버퍼의 마지막 (널 종료를 제외한) 요소가 되어야 한다.
        하지만, 아규먼트 벡터 버퍼의 크기가 이 인덱스보다 2 초과라면, 잘못된 입력이므로 에러메세지 출력 후 종료
        [..., 파일이름, NULL]로 아규먼트 벡터 버퍼가 정상적으로 저장되었다면, 파일 이름 요소를 그냥 NULL로 바꿔버리면 올바른 커맨드가 된다.
        -> [..., NULL, NULL]
        */
        if (argv_size > file_name_token_index + 2) {
        // wrong input,
        return PARSER_STATUS_INVALID_INPUT;
        }
        command_info_out->output_redirection_file_name = argv_buffer[file_name_token_index];
        argv_buffer[file_name_token_index] = NULL;
        --argv_size;
    } else {
        command_info_out->output_redirection_file_name = NULL;
    }

    // input validation completed?

    // parse end argv_buffer는 이제 잘 들어가는디
    for (size_t i = 0; i < argv_size - 1; ++i) {
        assert(*argv_buffer[i] != '\0' && argv_buffer[i] != NULL);
        printf("%s\n", argv_buffer[i]);
    }

    if (*p_input == '\0') {
        // strsep이 &
        return PARSER_STATUS_COMMAND_REMAINING;
    } else {
        return PARSER_STATUS_PARSE_COMPLETE;
    }
}