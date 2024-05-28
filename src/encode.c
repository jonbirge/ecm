#include "ecm.h"

void encode_file(FILE *input, FILE *output, int verbose) {
    char buffer[BUFFER_SIZE];
    size_t n;

    if (verbose) {
        fprintf(stderr, "Encoding started...\n");
    }

    while ((n = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
        fwrite(buffer, 1, n, output);
    }

    if (verbose) {
        fprintf(stderr, "Encoding finished.\n");
    }
}
