#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#define STDIO_LIMIT (65536)

void print_if_matched(FILE* stream, const char* keyword, const size_t keyword_length);

int main(int argc, char* argv[]) 
{
    // using getline is too dangerous for edge cases
    // use file pointer, record beginning file pointer location and if word found,
    // reset file pointer to beginning and print to stdout until newline character
    if (argc <= 1) {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    char* keyword = argv[1];
    size_t keyword_length = strlen(keyword);

    //from stdio
    FILE* stream;
    if (argc == 2) {
        stream = stdin;
        char buffer[STDIO_LIMIT];
        while (fgets(buffer, STDIO_LIMIT, stream) != NULL) {
            
            for (int i = 0; i < STDIO_LIMIT; ++i) {
                if (buffer[i] == '\0') {
                    break;
                }
                if (buffer[i] == keyword[0]) {
                    /* 
                        stdio로 "this line also has words" 입력하고
                        "google" 입력하면, "google\0ine also has words\0" 이런식으로 버퍼가 형성되어서
                        아래 조건문을 통과해벌임 일단 널캐릭터 확인..
                    */
                    if (strncmp(buffer + i, keyword, keyword_length) == 0) {
                        printf("%s", buffer);
                        break;
                    }
                }
            }
        }
        // fgets 써서하면 stdin으로 개많이 들어오면 터지는뎅
    }

    //from file
    for (int i = 2; i < argc; ++i) {
        stream = fopen(argv[i], "r");

        if (stream == NULL) {
            printf("wgrep: cannot open file\n");
            return 1;
        }
        print_if_matched(stream, keyword, keyword_length);
        fclose(stream);
        stream = NULL;
    }

    return 0;
}

    

void print_if_matched(FILE* stream, const char* keyword, const size_t keyword_length)
{
    int file_pos = ftell(stream);
    int alphabet = fgetc(stream);
    while (true) {
        // find condition resetted, set file pointer to new position
        if (alphabet == '\n') {
            file_pos = ftell(stream); // file pointer is at next character of \n
        }

        alphabet = fgetc(stream);
        if (alphabet == EOF) {
            break;
        }

        bool is_matched = false;
        if (alphabet == keyword[0]) {
            // begin check
            is_matched = true;
            for (int j = 1; j < keyword_length; ++j) {
                alphabet = fgetc(stream);
                if (alphabet != keyword[j]) {
                    is_matched = false;
                    break;
                }
            }
        }

        if (is_matched) {
            // print this line
            if (fseek(stream, file_pos, SEEK_SET)) {
                // fseek failed
                perror("unknown error while fseek");
                exit(1);
            }
            while (true) {
                alphabet = fgetc(stream);
                putc(alphabet, stdout);
                if (alphabet == '\n') {
                    break;
                }
            }
        }
    }
}