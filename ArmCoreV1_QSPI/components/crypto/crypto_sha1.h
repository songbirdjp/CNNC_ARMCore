#ifndef __CRYPTO_SHA1_H__
#define __CRYPTO_SHA1_H__


#ifdef __cplusplus
extern "C" {
#endif

uint8_t crypto_sha1_cal(uint8_t *buf, uint32_t len, uint8_t *hash, uint32_t *hash_len);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_SHA1_H__ */