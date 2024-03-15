#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h> 

#include "stack.h"

#define MALLOC_BUFFER_SIZE (8192)

static bool s_fail_bit = false;


int main(int argc, char* argv[])
{
    FILE* in;
    FILE* out;
    


    // 귀찮아서... ㅈㅅ
    if (argc == 1)
    {
        in = stdin;
        out = stdout;
    }
    else if (argc == 2)
    {
        char* file_name = argv[1];
        in = fopen(file_name, "rb");
        if (in == NULL)
        {
            fprintf(stderr, "%s '%s'\n", "reverse: cannot open file", file_name);
            exit(1);
        }
        out = stdout;
    }
    else if (argc == 3)
    {
        char* input_file_name = argv[1];
        char* output_file_name = argv[2];

        if (strcmp(input_file_name, output_file_name) == 0)
        {
            fprintf(stderr, "%s\n", "reverse: input and output file must differ");
            exit(1);
        }

        in = fopen(input_file_name, "rb");
        if (in == NULL)
        {
            fprintf(stderr, "%s '%s'\n", "reverse: cannot open file", input_file_name);
            exit(1);
        }
        out = fopen(output_file_name, "wb");
        if (out == NULL)
        {
            fclose(in);
            fprintf(stderr, "%s '%s'\n", "reverse: cannot open file", output_file_name);
            exit(1);
        }

        // same sourcefile?
        struct stat in_stat, out_stat;
        fstat(fileno(in), &in_stat);
        fstat(fileno(out), &out_stat);

        if (in_stat.st_dev == out_stat.st_dev && in_stat.st_ino == out_stat.st_ino)
        {
            fclose(in);
            fclose(out);
            fprintf(stderr, "%s\n", "reverse: input and output file must differ");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "%s\n", "usage: reverse <input> <output>");
        exit(1);
    }

    // 버퍼를 만들어서 처리해야하나? 근데 line 수가 너무 길면 어떡하지? 그냥 \n 도 바이트로 읽어서 거꾸로 해버리면 상관 없는거 아닐까?
    // 뒤에서부터 1바이트씩 읽어서 스택 버퍼에 넣고
    // 1. 스택 버퍼가 꽉 차면 -> 그냥 출력
    // 2. '\n' 읽으면 스택에 있는것 출력하고 마지막에 \n 출력 ('\n' 은 스택에 넣지 않음)
    // 아니면.. 파일을 거꾸로 읽는게 힘드니까 그냥 input은 첨부터 읽고  output에 한줄 쓰고 포인터를 옮기는게 맞겠다. -> stdout이 안옮겨져..
    // 근데 파일을 거꾸로 읽어도 stdin도 fseek 안될거같음
    // malloc밖에 없다

    while (true)
    {
        size_t line_capacity = sizeof(char) * MALLOC_BUFFER_SIZE;
        char* malloc_buffer = malloc(line_capacity);
        if (malloc_buffer == NULL)
        {
            goto close_stream;
        }

        ssize_t read = getline(&malloc_buffer, &line_capacity, in);
        malloc_buffer[line_capacity] = '\0'; // 못믿겠다 꾀꼬리
        if (read == -1)
        {
            if (feof(in))
            {
                //normal
                break;
            }
            s_fail_bit = true;
            goto close_stream;
        }

        stack_push(malloc_buffer);

        if (stack_is_full())
        {
            // 나도 모르겠다..
            goto close_stream;
        }
    }

    while (!stack_is_empty())
    {
        char* line = stack_pop_malloc(); // not null asserted
        fprintf(out, "%s", line); // getline이 newline 캐릭터를 안없애는군.. 거짓말쟁이들
        free(line);
    }


close_stream:
    // what if close stdin or stdout?
    if (in != NULL)
    {
        if (fclose(in) == EOF)
        {
            perror("error while closing file");
            exit(1);
        }
        
    }

    if (out != NULL)
    {
        if(fclose(out) == EOF)
        {
            perror("error while closing file");
            exit(1);
        }
    }
    
    if (s_fail_bit)
    {
        //print error
        fprintf(stderr, "sorry! error\n");
        exit(1);
    }

    return 0;
}