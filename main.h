#ifndef MAIN_HEADER

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_ARGUMENTS (4)
#define BYTE_ARRAY_LENGTH (1 << 8) // 256 different bytes

typedef struct huff_node {
    char byte;
    unsigned long count;
    struct huff_node* left;
    struct huff_node* right;
} huff_node_t;

#endif // !MAIN_HEADER
