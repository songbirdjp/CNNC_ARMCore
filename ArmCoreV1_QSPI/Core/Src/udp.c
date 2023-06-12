//
// Created by caijiaheng on 2023/6/12.
//
#include <lwip/udp.h>
#include "udp.h"
#include "lwip.h"


//测试UDP发送和接收，新建DUP连接，绑定本地IP地址，端口使用8080
PACK_STRUCT_BEGIN
struct pdu_t {
    PACK_STRUCT_FIELD(u8_t version_operation);
    PACK_STRUCT_FIELD(u8_t port);
    PACK_STRUCT_FIELD(u16_t data);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

void recv_callback (void *arg, struct udp_pcb *pcb, struct pbuf *p,
                    const ip_addr_t *addr, u16_t port);



extern struct netif gnetif;


int UdpClientInit(void) {

    struct udp_pcb *pcb;
    err_t err;

    pcb = udp_new();

    if (pcb == NULL){//申请失败
        return -1;
    }
    else{
        err = udp_bind(pcb, IP_ADDR_ANY, LOCAL_PORT);
        if(err == ERR_OK){
            /* 注册报文处理回调 */
            udp_recv(pcb,(void*)recv_callback, NULL);
        }
        printf("local_port %d\r\n", pcb->local_port);
        if (err != ERR_OK) {

            udp_remove(pcb);

            return -1;
        }
    }


    return 0;
}

void recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                   const ip_addr_t *addr, u16_t port)
{
//    struct pbuf *q;
//    struct ip4_addr test_ipaddr;
//    uint8_t  *temp = (uint8_t *)addr;
//    IP4_ADDR(&test_ipaddr, temp[0], temp[1], temp[2], temp[3]); // 保存源IP


    udp_sendto(pcb,p,addr,port);
    pbuf_free(p);
//    pbuf_free(q);

}