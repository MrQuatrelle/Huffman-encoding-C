#include "main.h"
#include <assert.h>
#include <stdlib.h>

FILE* input_fd;
FILE* output_fd;
//
// NOTE: this + 1 is a hack so there is always a NULL after the array for the
// while condition in build_heap(). There is probably a better way to do this
huff_node_t** byte_nodes;

void print_usage(char* program_name) {
    printf("huffman usage:\n"
           "\t%s\t[SETTINGS]+\tinput_file\toutput_file\n\n"
           "SETTINGS:\n"
           "\t --- missing_settings ---\n",
           program_name);
}

// for debugging
void print_heap(huff_node_t* head, int level) {
    if (head->right)
        print_heap(head->right, level + 1);

    int i = 0;
    while (i < level) {
        printf("\t");
        i++;
    }
    printf("(%c, %lu)\n", head->byte, head->count);

    if (head->left)
        print_heap(head->left, level + 1);
}

void sort_nodes() {
    for (int i = 0; i < BYTE_ARRAY_LENGTH - 1; i++) {
        if (!byte_nodes[i])
            return;
        for (int j = 0; j < BYTE_ARRAY_LENGTH - i - 1; j++) {
            if (!byte_nodes[j + 1])
                break;
            if (byte_nodes[j]->count > byte_nodes[j + 1]->count) {
                huff_node_t* temp = byte_nodes[j];
                byte_nodes[j] = byte_nodes[j + 1];
                byte_nodes[j + 1] = temp;
            }
        }
    }
}

// as in group all the non-NULL indexes in the front of the array
void group_nodes() {
    for (int i = 0; i < BYTE_ARRAY_LENGTH - 1; i++) {
        for (int j = 0; j < BYTE_ARRAY_LENGTH - i - 1; j++) {
            if (!byte_nodes[j]) {
                byte_nodes[j] = byte_nodes[j + 1];
                byte_nodes[j + 1] = NULL;
            }
        }
    }
}

void build_heap() {
    do {
        sort_nodes();
        huff_node_t* sum_node = malloc(sizeof(huff_node_t));
        assert(sum_node);
        sum_node->count = byte_nodes[0]->count + byte_nodes[1]->count;
        sum_node->left = byte_nodes[0];
        sum_node->right = byte_nodes[1];
        byte_nodes[1] = sum_node;
        byte_nodes++;
    } while (byte_nodes[1] != NULL); // while length > 1 (in the end there
                                     // will only be the head of the heap in
                                     // the list)
    print_heap(byte_nodes[0], 0);
}

void prepare_nodes() {
    int c;
    while ((c = fgetc(input_fd)) != EOF) {
        if (!byte_nodes[c]) {
            byte_nodes[c] = malloc(sizeof(huff_node_t));
            assert(byte_nodes[c]);
            byte_nodes[c]->byte = c;
        }
        byte_nodes[c]->count++;
    }

    group_nodes();
    build_heap();
}

int main(int argc, char* argv[]) {
    if (argc != NUM_ARGUMENTS) {
        print_usage(argv[0]);
        return 1;
    }

    if ((input_fd = fopen(argv[2], "r")) == NULL) {
        printf("error opening input file \"%s\":\n\t%s\n", argv[2],
               strerror(errno));
        return 1;
    }

    if ((output_fd = fopen(argv[3], "w")) == NULL) {
        printf("error opening output file \"%s\":\n\t%s\n", argv[3],
               strerror(errno));
        fclose(input_fd);
        return 1;
    }

    byte_nodes = malloc(sizeof(huff_node_t*) * (BYTE_ARRAY_LENGTH + 1));
    assert(byte_nodes);

    prepare_nodes();

    fclose(input_fd);
    fclose(output_fd);
    return 0;
}
