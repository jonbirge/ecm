/**************************************************************************/
/*
** Decoder for ECM (Error Code Modeler) format.
** Copyright (C) 2002 Neill Corlett
** Copyright (C) 2024 Jonathan Birge
**
***************************************************************************/
/*
** Portability notes:
**
** - Assumes a 32-bit or higher integer size
** - No assumptions about byte order
** - No assumptions about struct packing
** - No unaligned memory access
**
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ecm.h"

void edc_computeblock_decode(
    const ecc_uint8 *src,
    ecc_uint16 size,
    ecc_uint8 *dest)
{
    ecc_uint32 edc = edc_partial_computeblock(0, src, size);
    dest[0] = (edc >> 0) & 0xFF;
    dest[1] = (edc >> 8) & 0xFF;
    dest[2] = (edc >> 16) & 0xFF;
    dest[3] = (edc >> 24) & 0xFF;
}

/***************************************************************************/
/*
** Compute ECC for a block (can do either P or Q)
*/
void ecc_computeblock_decode(
    ecc_uint8 *src,
    ecc_uint32 major_count,
    ecc_uint32 minor_count,
    ecc_uint32 major_mult,
    ecc_uint32 minor_inc,
    ecc_uint8 *dest)
{
    ecc_uint32 size = major_count * minor_count;
    ecc_uint32 major, minor;
    for (major = 0; major < major_count; major++)
    {
        ecc_uint32 index = (major >> 1) * major_mult + (major & 1);
        ecc_uint8 ecc_a = 0;
        ecc_uint8 ecc_b = 0;
        for (minor = 0; minor < minor_count; minor++)
        {
            ecc_uint8 temp = src[index];
            index += minor_inc;
            if (index >= size)
                index -= size;
            ecc_a ^= temp;
            ecc_b ^= temp;
            ecc_a = ecc_f_lut[ecc_a];
        }
        ecc_a = ecc_b_lut[ecc_f_lut[ecc_a] ^ ecc_b];

        dest[major] = ecc_a;
        dest[major + major_count] = ecc_a ^ ecc_b;
    }
}

/*
** Generate ECC P and Q codes for a block
*/
void ecc_generate_decode(
    ecc_uint8 *sector,
    int zeroaddress)
{
    ecc_uint8 address[4], i;
    /* Save the address and zero it out */
    if (zeroaddress)
        for (i = 0; i < 4; i++)
        {
            address[i] = sector[12 + i];
            sector[12 + i] = 0;
        }
    /* Compute ECC P code */
    ecc_computeblock_decode(sector + 0xC, 86, 24, 2, 86, sector + 0x81C);
    /* Compute ECC Q code */
    ecc_computeblock_decode(sector + 0xC, 52, 43, 86, 88, sector + 0x8C8);
    /* Restore the address */
    if (zeroaddress)
        for (i = 0; i < 4; i++)
            sector[12 + i] = address[i];
}

/***************************************************************************/
/*
** Generate ECC/EDC information for a sector (must be 2352 = 0x930 bytes)
** Returns 0 on success
*/
void eccedc_generate_decode(ecc_uint8 *sector, int type)
{
    ecc_uint32 i;
    switch (type)
    {
    case 1: /* Mode 1 */
        /* Compute EDC */
        edc_computeblock_decode(sector + 0x00, 0x810, sector + 0x810);
        /* Write out zero bytes */
        for (i = 0; i < 8; i++)
            sector[0x814 + i] = 0;
        /* Generate ECC P/Q codes */
        ecc_generate_decode(sector, 0);
        break;
    case 2: /* Mode 2 form 1 */
        /* Compute EDC */
        edc_computeblock_decode(sector + 0x10, 0x808, sector + 0x818);
        /* Generate ECC P/Q codes */
        ecc_generate_decode(sector, 1);
        break;
    case 3: /* Mode 2 form 2 */
        /* Compute EDC */
        edc_computeblock_decode(sector + 0x10, 0x91C, sector + 0x92C);
        break;
    }
}

unsigned long mycounter_decode;
unsigned long mycounter_total_decode;

void resetcounter_decode(unsigned long total)
{
    mycounter_decode = 0;
    mycounter_total_decode = total;
}

void setcounter_decode(unsigned long n, int verbose)
{
    if ((n >> 20) != (mycounter_decode >> 20))
    {
        unsigned long a = (n + 64) / 128;
        unsigned long d = (mycounter_total_decode + 64) / 128;
        if (!d)
            d = 1;
        if (verbose)
        fprintf(stderr, "Decoding (%02lu%%)\r", (100 * a) / d);
    }
    mycounter_decode = n;
}

int decode_file(FILE *in, FILE *out, int verbose)
{
    unsigned checkedc = 0;
    unsigned char sector[2352];
    unsigned type;
    unsigned num;
    fseek(in, 0, SEEK_END);
    resetcounter_decode(ftell(in));
    fseek(in, 0, SEEK_SET);
    if (
        (fgetc(in) != 'E') ||
        (fgetc(in) != 'C') ||
        (fgetc(in) != 'M') ||
        (fgetc(in) != 0x00))
    {
        fprintf(stderr, "Header not found!\n");
        goto corrupt;
    }
    for (;;)
    {
        int c = fgetc(in);
        int bits = 5;
        if (c == EOF)
            goto uneof;
        type = c & 3;
        num = (c >> 2) & 0x1F;
        while (c & 0x80)
        {
            c = fgetc(in);
            if (c == EOF)
                goto uneof;
            num |= ((unsigned)(c & 0x7F)) << bits;
            bits += 7;
        }
        if (num == 0xFFFFFFFF)
            break;
        num++;
        if (num >= 0x80000000)
            goto corrupt;
        if (!type)
        {
            while (num)
            {
                int b = num;
                if (b > 2352)
                    b = 2352;
                if (fread(sector, 1, b, in) != b)
                    goto uneof;
                checkedc = edc_partial_computeblock(checkedc, sector, b);
                fwrite(sector, 1, b, out);
                num -= b;
                setcounter_decode(ftell(in), verbose);
            }
        }
        else
        {
            while (num--)
            {
                memset(sector, 0, sizeof(sector));
                memset(sector + 1, 0xFF, 10);
                switch (type)
                {
                case 1:
                    sector[0x0F] = 0x01;
                    if (fread(sector + 0x00C, 1, 0x003, in) != 0x003)
                        goto uneof;
                    if (fread(sector + 0x010, 1, 0x800, in) != 0x800)
                        goto uneof;
                    eccedc_generate_decode(sector, 1);
                    checkedc = edc_partial_computeblock(checkedc, sector, 2352);
                    fwrite(sector, 2352, 1, out);
                    setcounter_decode(ftell(in), verbose);
                    break;
                case 2:
                    sector[0x0F] = 0x02;
                    if (fread(sector + 0x014, 1, 0x804, in) != 0x804)
                        goto uneof;
                    sector[0x10] = sector[0x14];
                    sector[0x11] = sector[0x15];
                    sector[0x12] = sector[0x16];
                    sector[0x13] = sector[0x17];
                    eccedc_generate_decode(sector, 2);
                    checkedc = edc_partial_computeblock(checkedc, sector + 0x10, 2336);
                    fwrite(sector + 0x10, 2336, 1, out);
                    setcounter_decode(ftell(in), verbose);
                    break;
                case 3:
                    sector[0x0F] = 0x02;
                    if (fread(sector + 0x014, 1, 0x918, in) != 0x918)
                        goto uneof;
                    sector[0x10] = sector[0x14];
                    sector[0x11] = sector[0x15];
                    sector[0x12] = sector[0x16];
                    sector[0x13] = sector[0x17];
                    eccedc_generate_decode(sector, 3);
                    checkedc = edc_partial_computeblock(checkedc, sector + 0x10, 2336);
                    fwrite(sector + 0x10, 2336, 1, out);
                    setcounter_decode(ftell(in), verbose);
                    break;
                }
            }
        }
    }
    if (fread(sector, 1, 4, in) != 4)
        goto uneof;
    if (verbose)
        fprintf(stderr, "Decoded %ld bytes -> %ld bytes\n", ftell(in), ftell(out));
    if (
        (sector[0] != ((checkedc >> 0) & 0xFF)) ||
        (sector[1] != ((checkedc >> 8) & 0xFF)) ||
        (sector[2] != ((checkedc >> 16) & 0xFF)) ||
        (sector[3] != ((checkedc >> 24) & 0xFF)))
    {
        if (verbose)
            fprintf(stderr,
                    "EDC error (%08X, should be %02X%02X%02X%02X)\n",
                    checkedc,
                    sector[3],
                    sector[2],
                    sector[1],
                    sector[0]);
        goto corrupt;
    }
    if (verbose)
        fprintf(stderr, "Done; file is OK\n");
    return 0;
uneof:
    if (verbose)
        fprintf(stderr, "Unexpected EOF!\n");
corrupt:
    if (verbose)
        fprintf(stderr, "Corrupt ECM file!\n");
    return 1;
}
