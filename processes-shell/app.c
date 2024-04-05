#include "app.h"

static char* paths[1024] = { NULL, };

void error_handler()
{
    // TODO : clear all resources if possible,
    // TODO : kill all child process if possible
    fprintf(g_err_fd, "%s", s_error_message);
    exit(1);
}

int shell_init(int argc, char* argv[])
{   
    g_in_fd = stdin;
    g_out_fd = stdout;
    g_err_fd = stderr;

    // ㅋㅋ
    char* initial_path = malloc(5 * sizeof(char));
    strcpy(initial_path, "/bin");
    initial_path[4] = '\0';
    paths[0] = initial_path;

    if (argc == 1) {
        g_interactive_mode = true;
        return 1;
    } else if (argc == 2) {
        // batch mode, argv[1] should be input file;
        char* input_file_name = argv[1];
        assert (input_file_name != NULL);
        g_interactive_mode = false;
        g_in_fd = fopen(input_file_name, "r");
        if (g_in_fd == NULL) {
            return -1;
        }
        return 1;
    } else {
        return -1;
    }
}

void set_path(char** seperated_paths)
{
    // clear original paths[]
    size_t paths_idx = 0;
    while (paths[paths_idx] != NULL) {
        if (paths[paths_idx] != NULL) {
            free(paths[paths_idx]);
            paths[paths_idx] = NULL;
        }
    }

    // 괜히 원본 포인터 유지하려다가 귀찮아질수도 있으니까, strcpy해서 여기서 들고있자.
    paths_idx = 0;
    while (*seperated_paths != NULL) {
        char* original = *seperated_paths;
        size_t original_string_length = strlen(original);

        char* copy = malloc((original_string_length + 1)* sizeof(char));
        strcpy(copy, original);

        

        copy[original_string_length] = '\0';
        printf("new path %s set\n", copy);
        paths[paths_idx++] = copy;


        seperated_paths++;
    }

    
}

const char** const get_paths()
{
    return paths;        
}

