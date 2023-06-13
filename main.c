#include "main.h"

FILE* input_fd;
FILE* output_fd;

huff_node_t* byte_heap;

void print_usage(char* program_name) {
    printf("huffman usage:\n"
           "\t%s\t[SETTINGS]+\tinput_file\toutput_file\n\n"
           "SETTINGS:\n"
           "\t --- missing_settings ---\n",
           program_name);
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

    printf("(%c, %lu, %d, %u)\n", head->byte, head->count, head->level,
           head->code);

    if (head->left)
        print_heap(head->left, level + 1);
}

void
associate_codes_to_bytes(huff_node_t* head, char level, unsigned int code) {
    head->level = level;
    head->code = code;

    if (head->left)
        associate_codes_to_bytes(head->left, level + 1, code << 1);

    if (head->right)
        associate_codes_to_bytes(head->right, level + 1, (code << 1) + 1);
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
    associate_codes_to_bytes(heap[0], 0, 0);
    print_heap(heap[0], 0);

    byte_heap = heap[0];
}

void prepare_nodes() {
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

    free(byte_nodes);
    free(buffer);
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

    prepare_nodes();

    fclose(input_fd);
    fclose(output_fd);
    return 0;
}
