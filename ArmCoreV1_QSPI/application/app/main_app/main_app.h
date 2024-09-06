#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BEAM_NUM    30

struct ethercat_data_recv     /* master -> slave */
{
    /* data */
};

struct ethercat_data_send    /* slave -> master */
{
    /* data */
};

#define IS_TCP_SERVER

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_APP_H__ */