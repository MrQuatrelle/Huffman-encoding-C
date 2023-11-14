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
    // NOTE: e.g. for the code 010, I'ma do 0b1010, first positive bit marks
    // the beggining of the code. this helps knowing when I don't have any more
    // bites to decrypt, in case the last byte of the file isn't filled.
    unsigned int code;
    unsigned long count;
    struct huff_node* left;
    struct huff_node* right;
} huff_node_t;

#endif // !MAIN_HEADER
