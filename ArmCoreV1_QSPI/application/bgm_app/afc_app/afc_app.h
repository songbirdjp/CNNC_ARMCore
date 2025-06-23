#ifndef __AFC_APP_H__
#define __AFC_APP_H__


#ifdef __cplusplus
extern "C" {
#endif

enum afc_info_index
{
    AFC_INFO_VERSION = 0,
    AFC_INFO_MAG_POSITION,
};

float afc_info_get(enum afc_info_index index, void *data);

#ifdef __cplusplus
}
#endif

#endif /* __AFC_APP_H__ */