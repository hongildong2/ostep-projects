#include <stdio.h>
#include <string.h>
typedef struct {
    int size;
    char ab[6];
    char dummy[2];
} test_t;

static test_t test_arr[30];

void good(test_t input);

int main() {
    int size = 6;
    char* string22 = "abc";
    test_t test2 = {
        size,
        'a',
        'b',
        'c',
        'd',
        'e',
        '\0',
    };
    good(test2);

    printf("%d, %s\n", test_arr[0].size, test_arr[0].ab);
    printf("%lu\n", sizeof(test_t));
    return 0;
}

void good(test_t input)
{
    test_arr[0].size = input.size;
    test_arr[0].ab[0] = input.ab[0];
    test_arr[0].ab[1] = '\0';

}