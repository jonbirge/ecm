#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "ecm.h"

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [--decode|-d] [--output|-o outputfile] [--verbose|-v] [--help|-h] [inputfile]\n", prog_name);
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    FILE *output = stdout;
    int decode = 0;
    int verbose = 0;
    char *input_filename = NULL;
    int exit_code;

    char *prog_name = strrchr(argv[0], '/');
    prog_name = prog_name ? prog_name + 1 : argv[0];
    if (strcmp(prog_name, "unecm") == 0) {
        decode = 1;
    }

    struct option long_options[] = {
        {"decode", no_argument, 0, 'd'},
        {"output", required_argument, 0, 'o'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "dvo:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'd':
                decode = 1;
                break;
            case 'o':
                output = fopen(optarg, "w");
                if (output == NULL) {
                    perror("fopen");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        input_filename = argv[optind];
        input = fopen(input_filename, "r");
        if (input == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }

    if (decode) {
        exit_code = decode_file(input, output, verbose);
    } else {
        exit_code = encode_file(input, output, verbose);
    }

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return exit_code;
}
