#include <string.h>
#include "cmox_crypto.h"

uint8_t crypto_sha256_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len)
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
    retval = cmox_hash_compute(CMOX_SHA256_ALGO,                /* Use SHA256 algorithm */
                                buf, len,                       /* Message to digest */ 
                                hash,                           /* Data buffer to receive digest data */
                                CMOX_SHA256_SIZE,               /* Expected digest size */
                                &computed_size);                /* Size of computed digest */

    *hash_len = computed_size;

    /* Verify API returned value */
    if (retval != CMOX_HASH_SUCCESS)
    {
        printf("hash computation error %d\r\n", retval);
        return -3;
    }

    /* Verify generated data size is the expected one */
    if (computed_size != CMOX_SHA256_SIZE)
    {
        return -4;
    }

#if 0
    printf("cal hash: %u bytes\r\n", computed_size);

    for (uint32_t i = 0; i < computed_size; i++)
	{
		printf("%.2x ", hash[i]);
	}
    printf("\r\n");
#endif

#if 0
    /* Verify generated data are the expected ones */
    if (memcmp(Expected_Hash, computed_hash, computed_size) != 0)
    {
        Error_Handler();
    }
#endif

#if 0
  /* --------------------------------------------------------------------------
   * MULTIPLE CALLS USAGE
   * --------------------------------------------------------------------------
   */
    /* SHA256 context handle */
    cmox_sha256_handle_t sha256_ctx;

    /* General hash context */
    cmox_hash_handle_t *hash_ctx;

    /* Construct a hash context that is configured to perform SHA256 digest operations */
    hash_ctx = cmox_sha256_construct(&sha256_ctx);
    if (hash_ctx == NULL)
    {
        Error_Handler();
    }

    /* Initialize the hash context */
    retval = cmox_hash_init(hash_ctx);
    if (retval != CMOX_HASH_SUCCESS)
    {
        Error_Handler();
    }

    /* Set the desired size for the digest to compute: note that in the case
        where the size of the digest is the default for the algorithm, it is
        possible to skip this call. */
    retval = cmox_hash_setTagLen(hash_ctx, CMOX_SHA256_SIZE);
    if (retval != CMOX_HASH_SUCCESS)
    {
        Error_Handler();
    }

    #define CHUNK_SIZE  48u   /* Chunk size (in bytes) when data to hash are processed by chunk */

    /* Append the message to be hashed by chunks of CHUNK_SIZE Bytes */
    /* Index for piecemeal processing */
    uint32_t index;
    for (index = 0; index < (sizeof(Message) - CHUNK_SIZE); index += CHUNK_SIZE)
    {
        retval = cmox_hash_append(hash_ctx, &Message[index], CHUNK_SIZE); /* Chunk of data to digest */

        /* Verify API returned value */
        if (retval != CMOX_HASH_SUCCESS)
        {
            Error_Handler();
        }
    }
    /* Append the last part of the message if needed */
    if (index < sizeof(Message))
    {
        retval = cmox_hash_append(hash_ctx, &Message[index], sizeof(Message) - index); /* Last part of data to digest */

        /* Verify API returned value */
        if (retval != CMOX_HASH_SUCCESS)
        {
            Error_Handler();
        }
    }

    /* Generate the digest data */
    retval = cmox_hash_generateTag(hash_ctx, computed_hash, &computed_size);

    /* Verify API returned value */
    if (retval != CMOX_HASH_SUCCESS)
    {
        Error_Handler();
    }

    /* Verify generated data size is the expected one */
    if (computed_size != CMOX_SHA256_SIZE)
    {
        Error_Handler();
    }

    /* Verify generated data are the expected ones */
    if (memcmp(Expected_Hash, computed_hash, computed_size) != 0)
    {
        Error_Handler();
    }

    /* Cleanup the context */
    retval = cmox_hash_cleanup(hash_ctx);
    if (retval != CMOX_HASH_SUCCESS)
    {
        Error_Handler();
    }
#endif

    /* No more need of cryptographic services, finalize cryptographic library */
    if (cmox_finalize(NULL) != CMOX_INIT_SUCCESS)
    {
        printf("cryptographic library finalization failed\r\n");
        return -5;
    }

    return 0;
}

#ifdef CRYPTO_TEST
#include "stm32h7xx_hal.h"
#include "shell.h"
/* NOTE: verify the correctness of the generated hash with other tools: https://www.toolhelper.cn/DigestAlgorithm/SHA */

/** Extract from SHA256LongMsg.rsp
  *
Len = 2096
Msg = 6b918fb1a5ad1f9c5e5dbdf10a93a9c8f6bca89f37e79c9fe12a57227941b173ac79d8d440cde8c64c4ebc84a4c803
d198a296f3de060900cc427f58ca6ec373084f95dd6c7c427ecfbf781f68be572a88dbcbb188581ab200bfb99a3a816407e7
dd6dd21003554d4f7a99c93ebfce5c302ff0e11f26f83fe669acefb0c1bbb8b1e909bd14aa48ba3445c88b0e1190eef765ad
898ab8ca2fe507015f1578f10dce3c11a55fb9434ee6e9ad6cc0fdc4684447a9b3b156b908646360f24fec2d8fa69e2c93db
78708fcd2eef743dcb9353819b8d667c48ed54cd436fb1476598c4a1d7028e6f2ff50751db36ab6bc32435152a00abd3d58d
9a8770d9a3e52d5a3628ae3c9e0325
MD = 46500b6ae1ab40bde097ef168b0f3199049b55545a1588792d39d594f493dca7
  */
const uint8_t Message[] =
{
  0x6b, 0x91, 0x8f, 0xb1, 0xa5, 0xad, 0x1f, 0x9c, 0x5e, 0x5d, 0xbd, 0xf1, 0x0a, 0x93, 0xa9, 0xc8,
  0xf6, 0xbc, 0xa8, 0x9f, 0x37, 0xe7, 0x9c, 0x9f, 0xe1, 0x2a, 0x57, 0x22, 0x79, 0x41, 0xb1, 0x73,
  0xac, 0x79, 0xd8, 0xd4, 0x40, 0xcd, 0xe8, 0xc6, 0x4c, 0x4e, 0xbc, 0x84, 0xa4, 0xc8, 0x03, 0xd1,
  0x98, 0xa2, 0x96, 0xf3, 0xde, 0x06, 0x09, 0x00, 0xcc, 0x42, 0x7f, 0x58, 0xca, 0x6e, 0xc3, 0x73,
  0x08, 0x4f, 0x95, 0xdd, 0x6c, 0x7c, 0x42, 0x7e, 0xcf, 0xbf, 0x78, 0x1f, 0x68, 0xbe, 0x57, 0x2a,
  0x88, 0xdb, 0xcb, 0xb1, 0x88, 0x58, 0x1a, 0xb2, 0x00, 0xbf, 0xb9, 0x9a, 0x3a, 0x81, 0x64, 0x07,
  0xe7, 0xdd, 0x6d, 0xd2, 0x10, 0x03, 0x55, 0x4d, 0x4f, 0x7a, 0x99, 0xc9, 0x3e, 0xbf, 0xce, 0x5c,
  0x30, 0x2f, 0xf0, 0xe1, 0x1f, 0x26, 0xf8, 0x3f, 0xe6, 0x69, 0xac, 0xef, 0xb0, 0xc1, 0xbb, 0xb8,
  0xb1, 0xe9, 0x09, 0xbd, 0x14, 0xaa, 0x48, 0xba, 0x34, 0x45, 0xc8, 0x8b, 0x0e, 0x11, 0x90, 0xee,
  0xf7, 0x65, 0xad, 0x89, 0x8a, 0xb8, 0xca, 0x2f, 0xe5, 0x07, 0x01, 0x5f, 0x15, 0x78, 0xf1, 0x0d,
  0xce, 0x3c, 0x11, 0xa5, 0x5f, 0xb9, 0x43, 0x4e, 0xe6, 0xe9, 0xad, 0x6c, 0xc0, 0xfd, 0xc4, 0x68,
  0x44, 0x47, 0xa9, 0xb3, 0xb1, 0x56, 0xb9, 0x08, 0x64, 0x63, 0x60, 0xf2, 0x4f, 0xec, 0x2d, 0x8f,
  0xa6, 0x9e, 0x2c, 0x93, 0xdb, 0x78, 0x70, 0x8f, 0xcd, 0x2e, 0xef, 0x74, 0x3d, 0xcb, 0x93, 0x53,
  0x81, 0x9b, 0x8d, 0x66, 0x7c, 0x48, 0xed, 0x54, 0xcd, 0x43, 0x6f, 0xb1, 0x47, 0x65, 0x98, 0xc4,
  0xa1, 0xd7, 0x02, 0x8e, 0x6f, 0x2f, 0xf5, 0x07, 0x51, 0xdb, 0x36, 0xab, 0x6b, 0xc3, 0x24, 0x35,
  0x15, 0x2a, 0x00, 0xab, 0xd3, 0xd5, 0x8d, 0x9a, 0x87, 0x70, 0xd9, 0xa3, 0xe5, 0x2d, 0x5a, 0x36,
  0x28, 0xae, 0x3c, 0x9e, 0x03, 0x25
};

const uint8_t Expected_Hash[] =
{
  0x46, 0x50, 0x0b, 0x6a, 0xe1, 0xab, 0x40, 0xbd, 0xe0, 0x97, 0xef, 0x16, 0x8b, 0x0f, 0x31, 0x99,
  0x04, 0x9b, 0x55, 0x54, 0x5a, 0x15, 0x88, 0x79, 0x2d, 0x39, 0xd5, 0x94, 0xf4, 0x93, 0xdc, 0xa7
};

int8_t crypto_sha256_test(uint8_t argc, char *argv[])
{
    /* Computed data buffer */
    uint8_t computed_hash[CMOX_SHA256_SIZE] = {0};
    uint32_t computed_size = 0;
    int8_t ret = 0;

    if (argc != 2)
    {
        printf("Usage: crypto_sha256_test <message>\r\n");
        return -1;
    }

    switch (atoi(argv[1]))
    {
    case 0:
        ret = crypto_sha256_cal(Message, sizeof(Message), computed_hash, &computed_size);
        if (ret != 0)
        {
            printf("crypto_sha256_cal error %d\r\n", ret);
            return -2;
        }

        printf("cal sha256: %u bytes\r\n", computed_size);
        for (uint32_t i = 0; i < computed_size; i++)
        {
            printf("%.2x ", computed_hash[i]);
        }
        printf("\r\n");

        if (memcmp(Expected_Hash, computed_hash, computed_size) != 0)
        {
            printf("cal sha256: error\r\n");
            return -3;
        }
        else
        {
            printf("cal sha256: ok\r\n");
        }
        break;
    case 1:
        printf("stm32 uid:%#.8x%.8x%.8x\r\n", HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0());

        uint8_t msg_uid[12] = {0};
        memcpy(msg_uid, (uint32_t *)UID_BASE, 4);
        memcpy(msg_uid + 4, (uint32_t *)(UID_BASE + 4U), 4);
        memcpy(msg_uid + 8, (uint32_t *)(UID_BASE + 8U), 4);

        for (uint32_t i = 0; i < 12; i++)
        {
            printf("%.2x ", msg_uid[i]);
        }
        printf("\r\n");

        ret = crypto_sha256_cal(msg_uid, 12, computed_hash, &computed_size);
        if (ret != 0)
        {
            printf("crypto_sha256_cal error %d\r\n", ret);
            return -3;
        }

        printf("cal uid sha256: %u bytes\r\n", computed_size);
        for (uint32_t i = 0; i < computed_size; i++)
        {
            printf("%.2x ", computed_hash[i]);
        }
        printf("\r\n");
        break;
    default:
        break;
    }

    return 0;

}
MSH_CMD_EXPORT_ALIAS(crypto_sha256_test, crypto_sha256_test, test sha256);
#endif