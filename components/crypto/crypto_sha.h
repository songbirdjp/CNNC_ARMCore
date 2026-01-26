#ifndef __CRYPTO_SHA_H__
#define __CRYPTO_SHA_H__


#ifdef __cplusplus
extern "C" {
#endif

int8_t crypto_sha1_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len);
int8_t crypto_sha256_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_SHA_H__ */