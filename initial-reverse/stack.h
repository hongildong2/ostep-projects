#ifndef STACK_H
#define STACK_H
#include <stdbool.h>

void stack_push(char* buffer_ptr);

char* stack_pop_malloc();

bool stack_is_full();

bool stack_is_empty();


#endif /* STACK_H */