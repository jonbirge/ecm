#ifndef ECM_H
#define ECM_H

#include <stdio.h>

#define BUFFER_SIZE 1024

void print_usage(const char *prog_name);
void encode_file(FILE *input, FILE *output, int verbose);
void decode_file(FILE *input, FILE *output, int verbose);

#endif /* ECM_H */
