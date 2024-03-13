#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("%s\n", "wunzip: file1 [file2 ...]");
        exit(1);
    }


    for (int i = 1; i < argc; ++i)
    {
        char* file_name = argv[i];
        FILE* file = fopen(file_name, "rb");

        if (file == NULL)
        {
            printf("%s\n", "file not found");
            exit(1);
        }

        int read_count;
        char read_byte;
        size_t count_count = fread(&read_count, sizeof(int), 1, file);
        size_t count_byte = fread(&read_byte, sizeof(char), 1, file);

        while (count_count > 0 && count_byte > 0)
        {
            for (int j = 0; j < read_count; ++j)
            {
                printf("%c", read_byte);
            }

            count_count = fread(&read_count, sizeof(int), 1, file);
            count_byte = fread(&read_byte, sizeof(char), 1, file);
        }
    }



    return 0;
}