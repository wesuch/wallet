#ifndef _MICRO_ECC_H_
#define _MICRO_ECC_H_

#include <stdint.h>

#define NUM_ECC_DIGITS 32

typedef struct EccPoint
{
    unsigned char x[NUM_ECC_DIGITS];
    unsigned char y[NUM_ECC_DIGITS];
} EccPoint;

/* ecc_make_key() function.
Create a public/private key pair.

You must use a new nonpredictable random number to generate each new key pair.

Outputs:
    p_publicKey  - Will be filled in with the point representing the public key.
    p_privateKey - Will be filled in with the private key.

Inputs:
    p_random - The random number to use to generate the key pair.

Returns 1 if the key pair was generated successfully, 0 if an error occurred. If 0 is returned,
try again with a different random number.
*/
int ecc_make_key(EccPoint *p_publicKey, uint8_t p_privateKey[NUM_ECC_DIGITS], uint8_t p_random[NUM_ECC_DIGITS]);

#endif /* _MICRO_ECC_H_ */
