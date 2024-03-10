#include <libc.h>
#define BUFFER_SIZE (1024)

int main(int argc, char* argv[])
{
    if (argc == 1) {
        return 0;
    }

    char buffer[BUFFER_SIZE] = {0, };

    for (int i = 1; i < argc; ++i) {
        char* file_name = argv[i];
        FILE* file = fopen(file_name, "r");
        

        if (file == NULL) {
            printf("%s", "wcat: cannot open file\n");
            return 1;
        }
        
        while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
            printf("%s", buffer);
        }

        if (fclose(file)) {
            perror("error while closing");
        }
    }

    return 0;
}