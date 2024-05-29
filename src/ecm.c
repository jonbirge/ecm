#include "ecm.h"

/* Globals */
ecc_uint8 ecc_f_lut[256];
ecc_uint8 ecc_b_lut[256];
ecc_uint32 edc_lut[256];

/* Init routine */
void eccedc_init(void)
{
    ecc_uint32 i, j, edc;
    for (i = 0; i < 256; i++)
    {
        j = (i << 1) ^ (i & 0x80 ? 0x11D : 0);
        ecc_f_lut[i] = j;
        ecc_b_lut[i ^ j] = i;
        edc = i;
        for (j = 0; j < 8; j++)
            edc = (edc >> 1) ^ (edc & 1 ? 0xD8018001 : 0);
        edc_lut[i] = edc;
    }
}

/***************************************************************************/
/*
** Compute EDC for a block
*/
ecc_uint32 edc_partial_computeblock(
    ecc_uint32 edc,
    const ecc_uint8 *src,
    ecc_uint16 size)
{
    while (size--)
        edc = (edc >> 8) ^ edc_lut[(edc ^ (*src++)) & 0xFF];
    return edc;
}
