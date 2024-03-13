#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("%s\n", "wzip: file1 [file2 ...]");
        exit(1);
    }

    char read_byte;
    int count = 0;
    for (int i = 1; i < argc; ++i) {
        FILE* file = fopen(argv[i], "rb");

        if (file == NULL)
        {
            printf("%s\n", "file not found");
            exit(1);    
        }
        
        size_t read_count = fread(&read_byte, sizeof(char), 1, file);
        char prev_byte = read_byte;
        while (read_count != 0)
        {
            if (read_byte != prev_byte)
            {
                // record count and initialize variables for next loop

                // fwrite byte and count
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&prev_byte, sizeof(char), 1, stdout);

                // init vars
                prev_byte = read_byte;
                count = 0;
            }
            ++count;
            read_count = fread(&read_byte, sizeof(char), 1, file);
        }
    }

    // flush last element
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&read_byte, sizeof(char), 1, stdout);

    return 0;
}