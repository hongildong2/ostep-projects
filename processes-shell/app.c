#include "app.h"

// TODO : path도 여기서 관리하기.. malloc 해야할듯

int shell_init(int argc, char* argv[])
{   
    g_in_fd = stdin;
    g_out_fd = stdout;
    g_err_fd = stderr;
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

