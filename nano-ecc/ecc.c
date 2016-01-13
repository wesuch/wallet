#include "ecc.h"

#include <string.h>
#include <stdlib.h>

static unsigned char curve_p[NUM_ECC_DIGITS] = { \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

static EccPoint curve_G = { \
    {0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4, 0xA0, 0x33, 0xEB, 0x2D, \
        0x81, 0x7D, 0x03, 0x77, 0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, \
        0xF8, 0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B}, \
    {0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB, 0xCE, 0x5E, 0x31, 0x6B, \
        0x57, 0x33, 0xCE, 0x2B, 0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, \
        0x8E, 0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F}};

static unsigned char curve_n[NUM_ECC_DIGITS] = { \
    0x51, 0x25, 0x63, 0xFC, 0xC2, 0xCA, 0xB9, 0xF3, 0x84, \
    0x9E, 0x17, 0xA7, 0xAD, 0xFA, 0xE6, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

static void vli_clear(unsigned char *p_vli)
{
    unsigned int i;
    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        p_vli[i] = 0;
    }
}

/* Returns 1 if p_vli == 0, 0 otherwise. */
static int vli_isZero(unsigned char *p_vli)
{
    unsigned int i;
    for(i = 0; i < NUM_ECC_DIGITS; ++i)
    {
        if(p_vli[i])
        {
            return 0;
        }
    }
    return 1;
}

/* Returns nonzero if bit p_bit of p_vli is set. */
static unsigned char vli_testBit(unsigned char *p_vli, unsigned int p_bit)
{
    return (p_vli[p_bit/8] & (1 << (p_bit % 8)));
}

/* Counts the number of 8-bit "digits" in p_vli. */
static unsigned int vli_numDigits(unsigned char *p_vli)
{
    int i;
    /* Search from the end until we find a non-zero digit.
       We do it in reverse because we expect that most digits will be nonzero. */
    for(i = NUM_ECC_DIGITS - 1; i >= 0 && p_vli[i] == 0; --i)
    {
    }

    return (i + 1);
}

/* Counts the number of bits required for p_vli. */
static unsigned int vli_numBits(unsigned char *p_vli)
{
    unsigned int i;
    unsigned char l_digit;

    unsigned int l_numDigits = vli_numDigits(p_vli);
    if(l_numDigits == 0)
    {
        return 0;
    }

    l_digit = p_vli[l_numDigits - 1];
    for(i=0; l_digit; ++i)
    {
        l_digit >>= 1;
    }

    return ((l_numDigits - 1) * 8 + i);
}

/* Sets p_dest = p_src. */
static void vli_set(unsigned char *p_dest, unsigned char *p_src)
{
    unsigned int i;
    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        p_dest[i] = p_src[i];
    }
}

/* Returns sign of p_left - p_right. */
static int vli_cmp(unsigned char *p_left, unsigned char *p_right)
{
    int i;
    for(i = NUM_ECC_DIGITS-1; i >= 0; --i)
    {
        if(p_left[i] > p_right[i])
        {
            return 1;
        }
        else if(p_left[i] < p_right[i])
        {
            return -1;
        }
    }
    return 0;
}

/* Computes p_result = p_in << c, returning carry. Can modify in place (if p_result == p_in). 0 < p_shift < 8. */
static unsigned char vli_lshift(unsigned char *p_result, unsigned char *p_in, unsigned int p_shift)
{
    unsigned char l_carry = 0;
    unsigned int i;
    for(i = 0; i < NUM_ECC_DIGITS; ++i)
    {
        unsigned char l_temp = p_in[i];
        p_result[i] = (l_temp << p_shift) | l_carry;
        l_carry = l_temp >> (8 - p_shift);
    }

    return l_carry;
}

/* Computes p_vli = p_vli >> 1. */
static void vli_rshift1(unsigned char *p_vli)
{
    unsigned char *l_end = p_vli;
    unsigned char l_carry = 0;

    p_vli += NUM_ECC_DIGITS;
    while(p_vli-- > l_end)
    {
        unsigned char l_temp = *p_vli;
        *p_vli = (l_temp >> 1) | l_carry;
        l_carry = l_temp << 7;
    }
}

/* Computes p_result = p_left + p_right, returning carry. Can modify in place. */
static unsigned char vli_add(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right)
{
    unsigned char l_carry = 0;
    unsigned int i;
    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        unsigned char l_sum = p_left[i] + p_right[i] + l_carry;
        if(l_sum != p_left[i])
        {
            l_carry = (l_sum < p_left[i]);
        }
        p_result[i] = l_sum;
    }
    return l_carry;
}

/* Computes p_result = p_left - p_right, returning borrow. Can modify in place. */
static unsigned char vli_sub(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right)
{
    unsigned char l_borrow = 0;
    unsigned int i;
    for(i=0; i<NUM_ECC_DIGITS; ++i)
    {
        unsigned char l_diff = p_left[i] - p_right[i] - l_borrow;
        if(l_diff != p_left[i])
        {
            l_borrow = (l_diff > p_left[i]);
        }
        p_result[i] = l_diff;
    }
    return l_borrow;
}

/* Computes p_result = p_left * p_right. */
static void vli_mult(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right)
{
    uint16_t r01 = 0;
    unsigned char r2 = 0;

    unsigned int i, k;

    /* Compute each digit of p_result in sequence, maintaining the carries. */
    for(k=0; k < NUM_ECC_DIGITS*2 - 1; ++k)
    {
        unsigned int l_min = (k < NUM_ECC_DIGITS ? 0 : (k + 1) - NUM_ECC_DIGITS);
        for(i=l_min; i<=k && i<NUM_ECC_DIGITS; ++i)
        {
            uint16_t l_product = (uint16_t)p_left[i] * p_right[k-i];
            r01 += l_product;
            r2 += (r01 < l_product);
        }
        p_result[k] = (unsigned char)r01;
        r01 = (r01 >> 8) | (((uint16_t)r2) << 8);
        r2 = 0;
    }

    p_result[NUM_ECC_DIGITS*2 - 1] = (unsigned char)r01;
}

/* Computes p_result = (p_left + p_right) % p_mod.
   Assumes that p_left < p_mod and p_right < p_mod, p_result != p_mod. */
static void vli_modAdd(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right, unsigned char *p_mod)
{
    unsigned char l_carry = vli_add(p_result, p_left, p_right);
    if(l_carry || vli_cmp(p_result, p_mod) >= 0)
    { /* p_result > p_mod (p_result = p_mod + remainder), so subtract p_mod to get remainder. */
        vli_sub(p_result, p_result, p_mod);
    }
}

/* Computes p_result = (p_left - p_right) % p_mod.
   Assumes that p_left < p_mod and p_right < p_mod, p_result != p_mod. */
static void vli_modSub(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right, unsigned char *p_mod)
{
    unsigned char l_borrow = vli_sub(p_result, p_left, p_right);
    if(l_borrow)
    { /* In this case, p_result == -diff == (max int) - diff.
         Since -x % d == d - x, we can get the correct result from p_result + p_mod (with overflow). */
        vli_add(p_result, p_result, p_mod);
    }
}

/* Computes p_result = p_product % curve_p
   from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void vli_mmod_fast(unsigned char *p_result, unsigned char *p_product)
{
    unsigned char l_tmp[NUM_ECC_DIGITS];
    int l_carry;

    /* t */
    vli_set(p_result, p_product);

    /* s1 */
    l_tmp[0]  = l_tmp[1] = l_tmp[2]  = l_tmp[3] = 0;
    l_tmp[4]  = l_tmp[5] = l_tmp[6]  = l_tmp[7] = 0;
    l_tmp[8]  = l_tmp[9] = l_tmp[10] = l_tmp[11] = 0;
    l_tmp[12] = p_product[44];
    l_tmp[13] = p_product[45];
    l_tmp[14] = p_product[46];
    l_tmp[15] = p_product[47];
    l_tmp[16] = p_product[48];
    l_tmp[17] = p_product[49];
    l_tmp[18] = p_product[50];
    l_tmp[19] = p_product[51];
    l_tmp[20] = p_product[52];
    l_tmp[21] = p_product[53];
    l_tmp[22] = p_product[54];
    l_tmp[23] = p_product[55];
    l_tmp[24] = p_product[56];
    l_tmp[25] = p_product[57];
    l_tmp[26] = p_product[58];
    l_tmp[27] = p_product[59];
    l_tmp[28] = p_product[60];
    l_tmp[29] = p_product[61];
    l_tmp[30] = p_product[62];
    l_tmp[31] = p_product[63];
    l_carry = vli_lshift(l_tmp, l_tmp, 1);
    l_carry += vli_add(p_result, p_result, l_tmp);

    /* s2 */
    l_tmp[12] = p_product[48];
    l_tmp[13] = p_product[49];
    l_tmp[14] = p_product[50];
    l_tmp[15] = p_product[51];
    l_tmp[16] = p_product[52];
    l_tmp[17] = p_product[53];
    l_tmp[18] = p_product[54];
    l_tmp[19] = p_product[55];
    l_tmp[20] = p_product[56];
    l_tmp[21] = p_product[57];
    l_tmp[22] = p_product[58];
    l_tmp[23] = p_product[59];
    l_tmp[24] = p_product[60];
    l_tmp[25] = p_product[61];
    l_tmp[26] = p_product[62];
    l_tmp[27] = p_product[63];
    l_tmp[28] = 0;
    l_tmp[29] = 0;
    l_tmp[30] = 0;
    l_tmp[31] = 0;
    l_carry += vli_lshift(l_tmp, l_tmp, 1);
    l_carry += vli_add(p_result, p_result, l_tmp);

    /* s3 */
    l_tmp[0]  = p_product[32];
    l_tmp[1]  = p_product[33];
    l_tmp[2]  = p_product[34];
    l_tmp[3]  = p_product[35];
    l_tmp[4]  = p_product[36];
    l_tmp[5]  = p_product[37];
    l_tmp[6]  = p_product[38];
    l_tmp[7]  = p_product[39];
    l_tmp[8]  = p_product[40];
    l_tmp[9]  = p_product[41];
    l_tmp[10] = p_product[42];
    l_tmp[11] = p_product[43];
    l_tmp[12] = l_tmp[13] = l_tmp[14] = l_tmp[15] = 0;
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[56];
    l_tmp[25] = p_product[57];
    l_tmp[26] = p_product[58];
    l_tmp[27] = p_product[59];
    l_tmp[28] = p_product[60];
    l_tmp[29] = p_product[61];
    l_tmp[30] = p_product[62];
    l_tmp[31] = p_product[63];
    l_carry += vli_add(p_result, p_result, l_tmp);

    /* s4 */
    l_tmp[0]  = p_product[36];
    l_tmp[1]  = p_product[37];
    l_tmp[2]  = p_product[38];
    l_tmp[3]  = p_product[39];
    l_tmp[4]  = p_product[40];
    l_tmp[5]  = p_product[41];
    l_tmp[6]  = p_product[42];
    l_tmp[7]  = p_product[43];
    l_tmp[8]  = p_product[44];
    l_tmp[9]  = p_product[45];
    l_tmp[10] = p_product[46];
    l_tmp[11] = p_product[47];
    l_tmp[12] = p_product[52];
    l_tmp[13] = p_product[53];
    l_tmp[14] = p_product[54];
    l_tmp[15] = p_product[55];
    l_tmp[16] = p_product[56];
    l_tmp[17] = p_product[57];
    l_tmp[18] = p_product[58];
    l_tmp[19] = p_product[59];
    l_tmp[20] = p_product[60];
    l_tmp[21] = p_product[61];
    l_tmp[22] = p_product[62];
    l_tmp[23] = p_product[63];
    l_tmp[24] = p_product[52];
    l_tmp[25] = p_product[53];
    l_tmp[26] = p_product[54];
    l_tmp[27] = p_product[55];
    l_tmp[28] = p_product[32];
    l_tmp[29] = p_product[33];
    l_tmp[30] = p_product[34];
    l_tmp[31] = p_product[35];
    l_carry += vli_add(p_result, p_result, l_tmp);

    /* d1 */
    l_tmp[0]  = p_product[44];
    l_tmp[1]  = p_product[45];
    l_tmp[2]  = p_product[46];
    l_tmp[3]  = p_product[47];
    l_tmp[4]  = p_product[48];
    l_tmp[5]  = p_product[49];
    l_tmp[6]  = p_product[50];
    l_tmp[7]  = p_product[51];
    l_tmp[8]  = p_product[52];
    l_tmp[9]  = p_product[53];
    l_tmp[10] = p_product[54];
    l_tmp[11] = p_product[55];
    l_tmp[12] = l_tmp[13] = l_tmp[14] = l_tmp[15] = 0;
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[32];
    l_tmp[25] = p_product[33];
    l_tmp[26] = p_product[34];
    l_tmp[27] = p_product[35];
    l_tmp[28] = p_product[40];
    l_tmp[29] = p_product[41];
    l_tmp[30] = p_product[42];
    l_tmp[31] = p_product[43];
    l_carry -= vli_sub(p_result, p_result, l_tmp);

    /* d2 */
    l_tmp[0]  = p_product[48];
    l_tmp[1]  = p_product[49];
    l_tmp[2]  = p_product[50];
    l_tmp[3]  = p_product[51];
    l_tmp[4]  = p_product[52];
    l_tmp[5]  = p_product[53];
    l_tmp[6]  = p_product[54];
    l_tmp[7]  = p_product[55];
    l_tmp[8]  = p_product[56];
    l_tmp[9]  = p_product[57];
    l_tmp[10] = p_product[58];
    l_tmp[11] = p_product[59];
    l_tmp[12] = p_product[60];
    l_tmp[13] = p_product[61];
    l_tmp[14] = p_product[62];
    l_tmp[15] = p_product[63];
    l_tmp[16] = l_tmp[17] = l_tmp[18] = l_tmp[19] = 0;
    l_tmp[20] = l_tmp[21] = l_tmp[22] = l_tmp[23] = 0;
    l_tmp[24] = p_product[36];
    l_tmp[25] = p_product[37];
    l_tmp[26] = p_product[38];
    l_tmp[27] = p_product[39];
    l_tmp[28] = p_product[44];
    l_tmp[29] = p_product[45];
    l_tmp[30] = p_product[46];
    l_tmp[31] = p_product[47];
    l_carry -= vli_sub(p_result, p_result, l_tmp);

    /* d3 */
    l_tmp[0]  = p_product[52];
    l_tmp[1]  = p_product[53];
    l_tmp[2]  = p_product[54];
    l_tmp[3]  = p_product[55];
    l_tmp[4]  = p_product[56];
    l_tmp[5]  = p_product[57];
    l_tmp[6]  = p_product[58];
    l_tmp[7]  = p_product[59];
    l_tmp[8]  = p_product[60];
    l_tmp[9]  = p_product[61];
    l_tmp[10] = p_product[62];
    l_tmp[11] = p_product[63];
    l_tmp[12] = p_product[32];
    l_tmp[13] = p_product[33];
    l_tmp[14] = p_product[34];
    l_tmp[15] = p_product[35];
    l_tmp[16] = p_product[36];
    l_tmp[17] = p_product[37];
    l_tmp[18] = p_product[38];
    l_tmp[19] = p_product[39];
    l_tmp[20] = p_product[40];
    l_tmp[21] = p_product[41];
    l_tmp[22] = p_product[42];
    l_tmp[23] = p_product[43];
    l_tmp[24] = l_tmp[25] = l_tmp[26] = l_tmp[27] = 0;
    l_tmp[28] = p_product[48];
    l_tmp[29] = p_product[49];
    l_tmp[30] = p_product[50];
    l_tmp[31] = p_product[51];
    l_carry -= vli_sub(p_result, p_result, l_tmp);

    /* d4 */
    l_tmp[0]  = p_product[56];
    l_tmp[1]  = p_product[57];
    l_tmp[2]  = p_product[58];
    l_tmp[3]  = p_product[59];
    l_tmp[4]  = p_product[60];
    l_tmp[5]  = p_product[61];
    l_tmp[6]  = p_product[62];
    l_tmp[7]  = p_product[63];
    l_tmp[8]  = l_tmp[9]  = l_tmp[10]  = l_tmp[11] = 0;
    l_tmp[12] = p_product[36];
    l_tmp[13] = p_product[37];
    l_tmp[14] = p_product[38];
    l_tmp[15] = p_product[39];
    l_tmp[16] = p_product[40];
    l_tmp[17] = p_product[41];
    l_tmp[18] = p_product[42];
    l_tmp[19] = p_product[43];
    l_tmp[20] = p_product[44];
    l_tmp[21] = p_product[45];
    l_tmp[22] = p_product[46];
    l_tmp[23] = p_product[47];
    l_tmp[24] = l_tmp[25] = l_tmp[26] = l_tmp[27] = 0;
    l_tmp[28] = p_product[52];
    l_tmp[29] = p_product[53];
    l_tmp[30] = p_product[54];
    l_tmp[31] = p_product[55];
    l_carry -= vli_sub(p_result, p_result, l_tmp);

    if(l_carry < 0)
    {
        do
        {
            l_carry += vli_add(p_result, p_result, curve_p);
        } while(l_carry < 0);
    }
    else
    {
        while(l_carry || vli_cmp(curve_p, p_result) != 1)
        {
            l_carry -= vli_sub(p_result, p_result, curve_p);
        }
    }
}

/* Computes p_result = (p_left * p_right) % curve_p. */
static void vli_modMult_fast(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right)
{
   unsigned char l_product[2 * NUM_ECC_DIGITS];
   vli_mult(l_product, p_left, p_right);
   vli_mmod_fast(p_result, l_product);
}

#define vli_modSquare_fast(result, left) vli_modMult_fast((result), (left), (left))

#define EVEN(vli) (!(vli[0] & 1))
/* Computes p_result = (1 / p_input) % p_mod. All VLIs are the same size.
   See "From Euclid's GCD to Montgomery Multiplication to the Great Divide"
   https://labs.oracle.com/techrep/2001/smli_tr-2001-95.pdf */
static void vli_modInv(unsigned char *p_result, unsigned char *p_input, unsigned char *p_mod)
{
    unsigned char a[NUM_ECC_DIGITS], b[NUM_ECC_DIGITS], u[NUM_ECC_DIGITS], v[NUM_ECC_DIGITS];
    unsigned char l_carry;

    vli_set(a, p_input);
    vli_set(b, p_mod);
    vli_clear(u);
    u[0] = 1;
    vli_clear(v);

    int l_cmpResult;
    while((l_cmpResult = vli_cmp(a, b)) != 0)
    {
        l_carry = 0;
        if(EVEN(a))
        {
            vli_rshift1(a);
            if(!EVEN(u))
            {
                l_carry = vli_add(u, u, p_mod);
            }
            vli_rshift1(u);
            if(l_carry)
            {
                u[NUM_ECC_DIGITS-1] |= 0x80;
            }
        }
        else if(EVEN(b))
        {
            vli_rshift1(b);
            if(!EVEN(v))
            {
                l_carry = vli_add(v, v, p_mod);
            }
            vli_rshift1(v);
            if(l_carry)
            {
                v[NUM_ECC_DIGITS-1] |= 0x80;
            }
        }
        else if(l_cmpResult > 0)
        {
            vli_sub(a, a, b);
            vli_rshift1(a);
            if(vli_cmp(u, v) < 0)
            {
                vli_add(u, u, p_mod);
            }
            vli_sub(u, u, v);
            if(!EVEN(u))
            {
                l_carry = vli_add(u, u, p_mod);
            }
            vli_rshift1(u);
            if(l_carry)
            {
                u[NUM_ECC_DIGITS-1] |= 0x80;
            }
        }
        else
        {
            vli_sub(b, b, a);
            vli_rshift1(b);
            if(vli_cmp(v, u) < 0)
            {
                vli_add(v, v, p_mod);
            }
            vli_sub(v, v, u);
            if(!EVEN(v))
            {
                l_carry = vli_add(v, v, p_mod);
            }
            vli_rshift1(v);
            if(l_carry)
            {
                v[NUM_ECC_DIGITS-1] |= 0x80;
            }
        }
    }

    vli_set(p_result, u);
}

/* ------ Point operations ------ */

/* Returns 1 if p_point is the point at infinity, 0 otherwise. */
static int EccPoint_isZero(EccPoint *p_point)
{
    return (vli_isZero(p_point->x) && vli_isZero(p_point->y));
}

/* Point multiplication algorithm using Montgomery's ladder with co-Z coordinates.
From http://eprint.iacr.org/2011/338.pdf
*/

/* Double in place */
static void EccPoint_double_jacobian(unsigned char *X1, unsigned char *Y1, unsigned char *Z1)
{
    /* t1 = X, t2 = Y, t3 = Z */
    unsigned char t4[NUM_ECC_DIGITS];
    unsigned char t5[NUM_ECC_DIGITS];

    if(vli_isZero(Z1))
    {
        return;
    }

    vli_modSquare_fast(t4, Y1);   /* t4 = y1^2 */
    vli_modMult_fast(t5, X1, t4); /* t5 = x1*y1^2 = A */
    vli_modSquare_fast(t4, t4);   /* t4 = y1^4 */
    vli_modMult_fast(Y1, Y1, Z1); /* t2 = y1*z1 = z3 */
    vli_modSquare_fast(Z1, Z1);   /* t3 = z1^2 */

    vli_modAdd(X1, X1, Z1, curve_p); /* t1 = x1 + z1^2 */
    vli_modAdd(Z1, Z1, Z1, curve_p); /* t3 = 2*z1^2 */
    vli_modSub(Z1, X1, Z1, curve_p); /* t3 = x1 - z1^2 */
    vli_modMult_fast(X1, X1, Z1);    /* t1 = x1^2 - z1^4 */

    vli_modAdd(Z1, X1, X1, curve_p); /* t3 = 2*(x1^2 - z1^4) */
    vli_modAdd(X1, X1, Z1, curve_p); /* t1 = 3*(x1^2 - z1^4) */
    if(vli_testBit(X1, 0))
    {
        unsigned char l_carry = vli_add(X1, X1, curve_p);
        vli_rshift1(X1);
        X1[NUM_ECC_DIGITS-1] |= l_carry << 7;
    }
    else
    {
        vli_rshift1(X1);
    }
                     /* t1 = 3/2*(x1^2 - z1^4) = B */

    vli_modSquare_fast(Z1, X1);      /* t3 = B^2 */
    vli_modSub(Z1, Z1, t5, curve_p); /* t3 = B^2 - A */
    vli_modSub(Z1, Z1, t5, curve_p); /* t3 = B^2 - 2A = x3 */
    vli_modSub(t5, t5, Z1, curve_p); /* t5 = A - x3 */
    vli_modMult_fast(X1, X1, t5);    /* t1 = B * (A - x3) */
    vli_modSub(t4, X1, t4, curve_p); /* t4 = B * (A - x3) - y1^4 = y3 */

    vli_set(X1, Z1);
    vli_set(Z1, Y1);
    vli_set(Y1, t4);
}

/* Modify (x1, y1) => (x1 * z^2, y1 * z^3) */
static void apply_z(unsigned char *X1, unsigned char *Y1, unsigned char *Z)
{
    unsigned char t1[NUM_ECC_DIGITS];

    vli_modSquare_fast(t1, Z);    /* z^2 */
    vli_modMult_fast(X1, X1, t1); /* x1 * z^2 */
    vli_modMult_fast(t1, t1, Z);  /* z^3 */
    vli_modMult_fast(Y1, Y1, t1); /* y1 * z^3 */
}

/* P = (x1, y1) => 2P, (x2, y2) => P' */
static void XYcZ_initial_double(unsigned char *X1, unsigned char *Y1, unsigned char *X2, unsigned char *Y2, unsigned char *p_initialZ)
{
    unsigned char z[NUM_ECC_DIGITS];

    vli_set(X2, X1);
    vli_set(Y2, Y1);

    vli_clear(z);
    z[0] = 1;
    if(p_initialZ)
    {
        vli_set(z, p_initialZ);
    }
    apply_z(X1, Y1, z);

    EccPoint_double_jacobian(X1, Y1, z);

    apply_z(X2, Y2, z);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P' = (x1', y1', Z3), P + Q = (x3, y3, Z3)
   or P => P', Q => P + Q
*/
static void XYcZ_add(unsigned char *X1, unsigned char *Y1, unsigned char *X2, unsigned char *Y2)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    unsigned char t5[NUM_ECC_DIGITS];

    vli_modSub(t5, X2, X1, curve_p); /* t5 = x2 - x1 */
    vli_modSquare_fast(t5, t5);      /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5);    /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5);    /* t3 = x2*A = C */
    vli_modSub(Y2, Y2, Y1, curve_p); /* t4 = y2 - y1 */
    vli_modSquare_fast(t5, Y2);      /* t5 = (y2 - y1)^2 = D */

    vli_modSub(t5, t5, X1, curve_p); /* t5 = D - B */
    vli_modSub(t5, t5, X2, curve_p); /* t5 = D - B - C = x3 */
    vli_modSub(X2, X2, X1, curve_p); /* t3 = C - B */
    vli_modMult_fast(Y1, Y1, X2);    /* t2 = y1*(C - B) */
    vli_modSub(X2, X1, t5, curve_p); /* t3 = B - x3 */
    vli_modMult_fast(Y2, Y2, X2);    /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub(Y2, Y2, Y1, curve_p); /* t4 = y3 */

    vli_set(X2, t5);
}

/* Input P = (x1, y1, Z), Q = (x2, y2, Z)
   Output P + Q = (x3, y3, Z3), P - Q = (x3', y3', Z3)
   or P => P - Q, Q => P + Q
*/
static void XYcZ_addC(unsigned char *X1, unsigned char *Y1, unsigned char *X2, unsigned char *Y2)
{
    /* t1 = X1, t2 = Y1, t3 = X2, t4 = Y2 */
    unsigned char t5[NUM_ECC_DIGITS];
    unsigned char t6[NUM_ECC_DIGITS];
    unsigned char t7[NUM_ECC_DIGITS];

    vli_modSub(t5, X2, X1, curve_p); /* t5 = x2 - x1 */
    vli_modSquare_fast(t5, t5);      /* t5 = (x2 - x1)^2 = A */
    vli_modMult_fast(X1, X1, t5);    /* t1 = x1*A = B */
    vli_modMult_fast(X2, X2, t5);    /* t3 = x2*A = C */
    vli_modAdd(t5, Y2, Y1, curve_p); /* t4 = y2 + y1 */
    vli_modSub(Y2, Y2, Y1, curve_p); /* t4 = y2 - y1 */

    vli_modSub(t6, X2, X1, curve_p); /* t6 = C - B */
    vli_modMult_fast(Y1, Y1, t6);    /* t2 = y1 * (C - B) */
    vli_modAdd(t6, X1, X2, curve_p); /* t6 = B + C */
    vli_modSquare_fast(X2, Y2);      /* t3 = (y2 - y1)^2 */
    vli_modSub(X2, X2, t6, curve_p); /* t3 = x3 */

    vli_modSub(t7, X1, X2, curve_p); /* t7 = B - x3 */
    vli_modMult_fast(Y2, Y2, t7);    /* t4 = (y2 - y1)*(B - x3) */
    vli_modSub(Y2, Y2, Y1, curve_p); /* t4 = y3 */

    vli_modSquare_fast(t7, t5);      /* t7 = (y2 + y1)^2 = F */
    vli_modSub(t7, t7, t6, curve_p); /* t7 = x3' */
    vli_modSub(t6, t7, X1, curve_p); /* t6 = x3' - B */
    vli_modMult_fast(t6, t6, t5);    /* t6 = (y2 + y1)*(x3' - B) */
    vli_modSub(Y1, t6, Y1, curve_p); /* t2 = y3' */

    vli_set(X1, t7);
}

static void EccPoint_mult(EccPoint *p_result, EccPoint *p_point, unsigned char *p_scalar, unsigned char *p_initialZ)
{
    /* R0 and R1 */
    unsigned char Rx[2][NUM_ECC_DIGITS];
    unsigned char Ry[2][NUM_ECC_DIGITS];
    unsigned char z[NUM_ECC_DIGITS];

    unsigned int i, nb;

    vli_set(Rx[1], p_point->x);
    vli_set(Ry[1], p_point->y);

    XYcZ_initial_double(Rx[1], Ry[1], Rx[0], Ry[0], p_initialZ);

    for(i = vli_numBits(p_scalar) - 2; i > 0; --i)
    {
        nb = !vli_testBit(p_scalar, i);
        XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);
        XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);
    }

    nb = !vli_testBit(p_scalar, 0);
    XYcZ_addC(Rx[1-nb], Ry[1-nb], Rx[nb], Ry[nb]);

    /* Find final 1/Z value. */
    vli_modSub(z, Rx[1], Rx[0], curve_p); /* X1 - X0 */
    vli_modMult_fast(z, z, Ry[1-nb]);     /* Yb * (X1 - X0) */
    vli_modMult_fast(z, z, p_point->x);   /* xP * Yb * (X1 - X0) */
    vli_modInv(z, z, curve_p);            /* 1 / (xP * Yb * (X1 - X0)) */
    vli_modMult_fast(z, z, p_point->y);   /* yP / (xP * Yb * (X1 - X0)) */
    vli_modMult_fast(z, z, Rx[1-nb]);     /* Xb * yP / (xP * Yb * (X1 - X0)) */
    /* End 1/Z calculation */

    XYcZ_add(Rx[nb], Ry[nb], Rx[1-nb], Ry[1-nb]);

    apply_z(Rx[0], Ry[0], z);

    vli_set(p_result->x, Rx[0]);
    vli_set(p_result->y, Ry[0]);
}

int ecc_make_key(EccPoint *p_publicKey, unsigned char p_privateKey[NUM_ECC_DIGITS], unsigned char p_random[NUM_ECC_DIGITS])
{
    /* Make sure the private key is in the range [1, n-1].
       For the supported curves, n is always large enough that we only need to subtract once at most. */
    vli_set(p_privateKey, p_random);
    if(vli_cmp(curve_n, p_privateKey) != 1)
    {
        vli_sub(p_privateKey, p_privateKey, curve_n);
    }

    if(vli_isZero(p_privateKey))
    {
        return 0; /* The private key cannot be 0 (mod p). */
    }

    EccPoint_mult(p_publicKey, &curve_G, p_privateKey, NULL);
    return 1;
}

/* -------- ECDSA code -------- */

/* Computes p_result = (p_left * p_right) % p_mod. */
static void vli_modMult(unsigned char *p_result, unsigned char *p_left, unsigned char *p_right, unsigned char *p_mod)
{
    unsigned char l_product[2 * NUM_ECC_DIGITS];
    unsigned char l_modMultiple[2 * NUM_ECC_DIGITS];
    unsigned int l_digitShift, l_bitShift;
    unsigned int l_productBits;
    unsigned int l_modBits = vli_numBits(p_mod);
    
    vli_mult(l_product, p_left, p_right);
    l_productBits = vli_numBits(l_product + NUM_ECC_DIGITS);
    if(l_productBits)
    {
        l_productBits += NUM_ECC_DIGITS * 8;
    }
    else
    {
        l_productBits = vli_numBits(l_product);
    }
    
    if(l_productBits < l_modBits)
    { /* l_product < p_mod. */
        vli_set(p_result, l_product);
        return;
    }
    
    /* Shift p_mod by (l_leftBits - l_modBits). This multiplies p_mod by the largest
       power of two possible while still resulting in a number less than p_left. */
    vli_clear(l_modMultiple);
    vli_clear(l_modMultiple + NUM_ECC_DIGITS);
    l_digitShift = (l_productBits - l_modBits) / 8;
    l_bitShift = (l_productBits - l_modBits) % 8;
    if(l_bitShift)
    {
        l_modMultiple[l_digitShift + NUM_ECC_DIGITS] = vli_lshift(l_modMultiple + l_digitShift, p_mod, l_bitShift);
    }
    else
    {
        vli_set(l_modMultiple + l_digitShift, p_mod);
    }

    /* Subtract all multiples of p_mod to get the remainder. */
    vli_clear(p_result);
    p_result[0] = 1; /* Use p_result as a temp var to store 1 (for subtraction) */
    while(l_productBits > NUM_ECC_DIGITS * 8 || vli_cmp(l_modMultiple, p_mod) >= 0)
    {
        int l_cmp = vli_cmp(l_modMultiple + NUM_ECC_DIGITS, l_product + NUM_ECC_DIGITS);
        if(l_cmp < 0 || (l_cmp == 0 && vli_cmp(l_modMultiple, l_product) <= 0))
        {
            if(vli_sub(l_product, l_product, l_modMultiple))
            { /* borrow */
                vli_sub(l_product + NUM_ECC_DIGITS, l_product + NUM_ECC_DIGITS, p_result);
            }
            vli_sub(l_product + NUM_ECC_DIGITS, l_product + NUM_ECC_DIGITS, l_modMultiple + NUM_ECC_DIGITS);
        }
        unsigned char l_carry = (l_modMultiple[NUM_ECC_DIGITS] & 0x01) << 7;
        vli_rshift1(l_modMultiple + NUM_ECC_DIGITS);
        vli_rshift1(l_modMultiple);
        l_modMultiple[NUM_ECC_DIGITS-1] |= l_carry;
        
        --l_productBits;
    }
    vli_set(p_result, l_product);
}
