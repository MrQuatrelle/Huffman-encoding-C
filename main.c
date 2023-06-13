#include <stdio.h>
#include <errno.h>
#include <string.h>

#define NUM_ARGUMENTS 4

FILE* input_fd;
FILE* output_fd;
unsigned long char_counters[1 << 8];

void print_usage(char* program_name) {
    printf("huffman usage:\n"
           "\t%s\t[SETTINGS]+\tinput_file\toutput_file\n\n"
           "SETTINGS:\n"
           "\t --- missing_settings ---\n",
           program_name);
}

int main(int argc, char* argv[]) {
    if (argc != NUM_ARGUMENTS) {
        print_usage(argv[0]);
        return 1;
    }

    if ((input_fd = fopen(argv[2], "r")) == NULL) {
        printf("error opening input file \"%s\":\n\t%s\n",
               argv[2], strerror(errno));
        return 1;
    }

    if ((output_fd = fopen(argv[3], "w")) == NULL) {
        printf("error opening output file \"%s\":\n\t%s\n",
               argv[3], strerror(errno));
        fclose(input_fd);
        return 1;
    }

    fclose(input_fd);
    fclose(output_fd);
    return 0;
}
