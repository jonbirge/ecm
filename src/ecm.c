#include "ecm.h"

/* Init routine */
static void eccedc_init(void)
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
