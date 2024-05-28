#ifndef ECM_H
#define ECM_H

#include <stdio.h>

/* Data types */
typedef unsigned char ecc_uint8;
typedef unsigned short ecc_uint16;
typedef unsigned int ecc_uint32;

/* LUTs used for computing ECC/EDC */
extern ecc_uint8 ecc_f_lut[];
extern ecc_uint8 ecc_b_lut[];
extern ecc_uint32 edc_lut[];

/* Functions */
void print_usage(const char *prog_name);
void eccedc_init(void);
int encode_file(FILE *in, FILE *out, int verbose);
int decode_file(FILE *in, FILE *out, int verbose);

#endif /* ECM_H */
