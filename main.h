#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ARGUMENTS (4)
#define BYTE_ARRAY_LENGTH (1 << 8) // 256 different bytes

typedef struct huff_node {
    char byte;
    unsigned int code;
    // HACK: I don't see any other way of knowing how many bits there are
    // in the code...
    unsigned int level;
    unsigned long count;
    struct huff_node* left;
    struct huff_node* right;
} huff_node_t;

#endif // !MAIN_HEADER
