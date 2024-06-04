#ifndef __CRYPTO_SHA256_H__
#define __CRYPTO_SHA256_H__


#ifdef __cplusplus
extern "C" {
#endif




uint8_t crypto_sha256_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_SHA256_H__ */