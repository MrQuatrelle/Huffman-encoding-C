#include "main.h"
#include <stdio.h>
#include <sys/cdefs.h>

FILE* input_fd;
FILE* output_fd;

huff_node_t* byte_heap;
int byte_heap_depth = 0;

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

    {
        int i = 0;
        while (i < level) {
            printf("\t");
            i++;
        }
    }

    printf("(%c, %lu, ", head->byte, head->count);

    { // priting the code in binary form
        unsigned int mask = 1;
        while (mask <= head->code)
            mask <<= 1;
        mask >>= 1;

        while (mask) {
            printf("%d", !!(head->code & mask));
            mask >>= 1;
        }
    }

    printf(")\n");
    if (head->left)
        print_heap(head->left, level + 1);
}

void sort_nodes(huff_node_t* node_array[], int length) {
    for (int i = 0; i < length - 1; i++) {
        if (!node_array[i])
            return;
        for (int j = 0; j < length - i - 1; j++) {
            if (!node_array[j + 1])
                break;
            if (node_array[j]->count > node_array[j + 1]->count) {
                huff_node_t* temp = node_array[j];
                node_array[j] = node_array[j + 1];
                node_array[j + 1] = temp;
            }
        }
    }
}

void associate_codes_to_bytes(huff_node_t* head, unsigned code, int depth) {
    head->code = code;
    byte_heap_depth = (depth > byte_heap_depth) ? depth : byte_heap_depth;

    if (head->left)
        associate_codes_to_bytes(head->left, code << 1, depth + 1);

    if (head->right)
        associate_codes_to_bytes(head->right, (code << 1) + 1, depth + 1);
}

void build_heap(huff_node_t** heap, int length) {
    do {
        sort_nodes(heap, length);

        huff_node_t* sum_node = malloc(sizeof(huff_node_t));
        assert(sum_node);

        sum_node->count = heap[0]->count + heap[1]->count;
        sum_node->left = heap[0];
        sum_node->right = heap[1];
        heap[1] = sum_node;

        heap++;
    } while (heap[1] != NULL); // while length > 1 (in the end there
                               // will only be the head of the heap in
                               // the list)
    associate_codes_to_bytes(heap[0], 0b1, 1);

    // // TODO: remove debugging
    // print_heap(heap[0], 0);

    byte_heap = heap[0];
}

huff_node_t** prepare_nodes() {
    huff_node_t** byte_nodes = malloc(sizeof(huff_node_t*) * BYTE_ARRAY_LENGTH);
    assert(byte_nodes);

    int c;
    int unique_bytes = 0;
    while ((c = fgetc(input_fd)) != EOF) {
        if (!byte_nodes[c]) {
            byte_nodes[c] = malloc(sizeof(huff_node_t));
            assert(byte_nodes[c]);

            byte_nodes[c]->byte = c;
            unique_bytes++;
        }
        byte_nodes[c]->count++;
    }

    huff_node_t** buffer = malloc(sizeof(huff_node_t*) * unique_bytes);
    assert(buffer);
    int i = 0;

    for (int j = 0; j < BYTE_ARRAY_LENGTH; j++) {
        if (byte_nodes[j])
            buffer[i++] = byte_nodes[j];
    }

    build_heap(buffer, unique_bytes);
    free(buffer);

    return byte_nodes;
}

__always_inline unsigned int get_code(int c, huff_node_t** key_stack) {
    return key_stack[c]->code;
}

void generate_compressed_content(huff_node_t** key_stack) {
    // TODO: how the hell am I going to save the encoding in the file???

    char c;
    char buffer = 0;
    char counter = 0;
    while ((c = fgetc(input_fd)) != EOF) {
        unsigned int mask = 1 << 31;
        unsigned int key = get_code(c, key_stack);

        // advance until the start of the code
        while (!(key & mask))
            mask >>= 1;
        mask >>= 1;
        //

        while (mask) {
            if (mask & key)
                buffer |= 1;
            counter++;

            if (counter == 8) {
                fputc(buffer, output_fd);
                counter = 0;
                buffer = 0;
            } else {
                buffer <<= 1;
            }

            mask >>= 1;
        }
    }
    if (counter)
        fputc(buffer, output_fd);
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

    huff_node_t** key_stack = prepare_nodes();

    fseek(input_fd, 0, SEEK_SET);

    generate_compressed_content(key_stack);

    fclose(input_fd);
    fclose(output_fd);
    return 0;
}
