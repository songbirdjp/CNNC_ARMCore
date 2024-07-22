#include <string.h>
#include "cmox_crypto.h"

uint8_t crypto_sha1_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len)
{
    if (buf == NULL || len == 0 || hash == NULL || hash_len == NULL)
    {
        printf("parameter error\r\n");
        return -1;
    }

    /* Initialize cryptographic library */
    if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
    {
        printf("cryptographic library initialization failed\r\n");
        return -2;
    }

    /* --------------------------------------------------------------------------
    * SINGLE CALL USAGE
    * --------------------------------------------------------------------------
    */
   	cmox_hash_retval_t retval;
    size_t computed_size;
    
    /* Compute directly the digest passing all the needed parameters */
    retval = cmox_hash_compute(CMOX_SHA1_ALGO,                /* Use SHA256 algorithm */
                                buf, len,                       /* Message to digest */ 
                                hash,                           /* Data buffer to receive digest data */
                                CMOX_SHA1_SIZE,               /* Expected digest size */
                                &computed_size);                /* Size of computed digest */

    *hash_len = computed_size;

    /* Verify API returned value */
    if (retval != CMOX_HASH_SUCCESS)
    {
        printf("hash computation error %d\r\n", retval);
        return -3;
    }

    /* Verify generated data size is the expected one */
    if (computed_size != CMOX_SHA1_SIZE)
    {
        return -4;
    }

     /* No more need of cryptographic services, finalize cryptographic library */
    if (cmox_finalize(NULL) != CMOX_INIT_SUCCESS)
    {
        printf("cryptographic library finalization failed\r\n");
        return -5;
    }
  
    return 0;
}