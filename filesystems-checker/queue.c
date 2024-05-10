#include <stdio.h>

#include "queue.h"
#include "assert.h"

#define MAX_NUMS (1000)

static int s_buffer[MAX_NUMS];
static int s_front = 0;
static int s_back = 0;
static int s_num_count = 0;


void enqueue(int n)
{
    assert (s_num_count < MAX_NUMS);
    s_buffer[s_back] = n;
    s_back = (s_back + 1) % MAX_NUMS;
    s_num_count++;
}

int dequeue()
{
    assert (s_front != s_back);
    int n = s_buffer[s_front];
    s_front = (s_front + 1) % MAX_NUMS;
    s_num_count--;
    return n;
}

int is_empty()
{
    return s_num_count == 0;
}