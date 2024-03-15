#include <stdlib.h>
#include <assert.h>

#include "stack.h"


#define STACK_SIZE (16384)
static char* s_buffer[STACK_SIZE] = {0, };
static size_t s_current_index = -1;

void stack_push(char* line_ptr)
{
    assert(!stack_is_full() && line_ptr != NULL);
    s_buffer[++s_current_index] = line_ptr;
    assert (s_current_index < STACK_SIZE);
}

char* stack_pop_malloc()
{
    assert (!stack_is_empty());
    return s_buffer[s_current_index--];
}

bool stack_is_full()
{
    return s_current_index == STACK_SIZE - 1;
}

bool stack_is_empty()
{
    return s_current_index == -1;
}
