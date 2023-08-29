#include <stddef.h>
#include "main.h"
#include "wizchip_conf.h"
#include "retarget.h"
#include "socket.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "httpserver.h"

uint8_t DstIP[4] = {192, 168, 10, 100};//while stm32 is tcp client
uint16_t DstPort = 8000;

TCP_DATA_t recvInfo;
extern TCP_DATA_t RecvByUART;
uint32_t crtTick, oldTick;
#define SOCK_TCPS        0
#define RX_BUF_SIZE 512

uint8_t rxBuffer[RX_BUF_SIZE];
int _numbers[MAX_HTTPSEND_NUMBERS];

void do_tcpc(void)
{
    switch (getSn_SR(SOCK_TCPS))                  /*获取socket的状态*/
    {
        case SOCK_CLOSED:/*socket处于关闭状态*/
           // printf("TCP CLIENT START!\r\n");
            socket(0, Sn_MR_TCP, 8123, Sn_MR_ND);
          //  printf("SOCK OPEN SUCCESS!\r\n");
            break;
        case SOCK_INIT:                      /*socket处于初始化状态*/
          //  printf("try to connect %d.%d.%d.%d: %d...\r\n", DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
            connect(0, DstIP, DstPort);/*socket连接服务器*/
            //printf("CLIENT_SOCK_INIT\r\n");
            break;
        case SOCK_ESTABLISHED:               /*socket处于连接建立状态*/
            if (getSn_IR(0) & Sn_IR_CON)
            {
                printf("socket0: Connected to %d.%d.%d.%d: %d...\r\n", DstIP[0],DstIP[1],DstIP[2],DstIP[3],DstPort);
                setSn_IR(0, Sn_IR_CON);      /*清除接收中断标志位*/
            }
            recvInfo.Len = getSn_RX_RSR(0);            /*获取接收的数据长度*/

            if (recvInfo.Len > 0)  //接收到数据
            {
               // printf("socket0: Recv data %d bytes.\r\n", recvInfo.Len);
             //   crtTick = xTaskGetTickCount();
                printf("recv period = %d ms\r\n", (crtTick - oldTick)*portTICK_RATE_MS);
             //   oldTick = crtTick;

                recv(0, recvInfo.gDATABUF, recvInfo.Len);     /*接收来自Server的数据*/
                osMessageQueuePut(networkRecvQueueHandle, &recvInfo, 0, 10);
            }
            break;
        case SOCK_CLOSE_WAIT:        /*socket处于等待关闭状态*/
            close(0);
//            printf("SOCK_CLOSE_WAIT\r\n");
            break;
    }
}
#if 0
void do_tcp_server(void)
{
    uint8_t flag = 0;
    uint16_t len;
    switch (getSn_SR(SOCK_TCPS))
    {
        case SOCK_INIT:
            listen(SOCK_TCPS);
            printf("SERVER_SOCK_INIT\r\n");
            break;
        case SOCK_ESTABLISHED:
            if (getSn_IR(SOCK_TCPS) & Sn_IR_CON)
            {
                setSn_IR(SOCK_TCPS, Sn_IR_CON);
            }

            len = 4096;//TODO 这里是个坑，getSn_RX_RSR(SOCK_TCPS)取数总是问题，目前把长度写成4096各方面都是正常的，如果有问题先查这里。
            memset(gDATABUF, 0, sizeof(gDATABUF));
            recv(SOCK_TCPS, gDATABUF, len);
            if (len)
            {
                // 解析HTTP请求
                int http_request_type = parse_http_request(gDATABUF);
                printf("%s\r\n",gDATABUF);
                // 根据HTTP请求的类型，发送HTTP响应
                if (http_request_type == 1)
                {
                    printf("http_request_type = %x\r\n", http_request_type);
                    if(Get_IO_Flag(gDATABUF) == 1)//抓取HTTP响应头是否有io-state标志
                    {
                        handleIOStatusRequest(SOCK_TCPS);
                    }
                    else//没有io位即正常响应标准页面
                    {
                        SendHttpResponse_Get(SOCK_TCPS);
                    }

                }
                else if (http_request_type == 2)
                {
                    printf("http_request_type22222 = %x\r\n", http_request_type);

                    handle_request(gDATABUF);
                }
                else if (http_request_type == 4)
                {
                    // 处理DELETE请求的响应
                    // ...
                }
                else if (http_request_type == 5)
                {
                    // 处理HEAD请求的响应
                    // ...
                }
                else if (http_request_type == 6)
                {
                    // 处理OPTIONS请求的响应
                    // ...

                }
                else if (http_request_type == 7)
                {
                    // 处理PATCH请求的响应
                    // ...
                }
                else if (http_request_type == 8)
                {
                    // 处理TRACE请求的响应
                    // ...
                }
                else if (http_request_type == 9)
                {
                    // 处理CONNECT请求的响应
                    // ...
                }
                else if(http_request_type == 10)
                {
                    char httpResponse[] = "HTTP/1.1 Option type\r\n\r\n";
                    send(SOCK_TCPS, (uint8_t *) httpResponse, sizeof(httpResponse));
                    printf("%s\r\n",gDATABUF);
                    http_String2Numbers(gDATABUF);
                }
                else
                {
                    // 对于我们不支持的HTTP请求，返回"404 Not Found"错误
                    char httpResponse[] = "HTTP/1.1 404 Not Found\r\n\r\n";
                    send(SOCK_TCPS, (uint8_t *) httpResponse, sizeof(httpResponse));
                }
            }
            printf("SERVER_SOCKSOCK_ESTABLISHED\r\n");
            disconnect(SOCK_TCPS);
            break;
        case SOCK_CLOSE_WAIT:
            disconnect(SOCK_TCPS);
            printf("SERVER_SOCK_CLOSED_WAIT\r\n");
            break;
        case SOCK_CLOSED:
            socket(SOCK_TCPS, Sn_MR_TCP, 5000, 0x00);
            printf("SERVER_SOCK_CLOSED\r\n");
            break;
    }
}
#endif
///gDATABUFdo_tcp_server = GET /io-status HTTP/1.1
//Host: 192.168.100.72:5000
//User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36
//Accept: */*
//Accept-Encoding: gzip, deflate
//Accept-Language: zh-CN,zh;q=0.9
//Referer: http://192.168.100.72:5000/
//
//
//LEN = 1000
//http_request_type = 1
//SenfHttpResponse_Get

int parse_http_request(char *request)//HTTP请求解析函数
{
    // 检查是否是GET请求
    if (strncmp(request, "GET", 3) == 0)
    {
        return 1;
    }
        // 检查是否是POST请求
    else if (strncmp(request, "POST", 4) == 0)
    {
        return 2;
    }
    else if (strncmp(request, "PUT", 3) == 0)
    {
        return 3;
    }
    else if (strncmp(request, "DELETE", 6) == 0)
    {
        return 4;
    }
    else if (strncmp(request, "HEAD", 4) == 0)
    {
        return 5;
    }
    else if (strncmp(request, "OPTIONS", 7) == 0)
    {
        return 6;
    }
    else if (strncmp(request, "PATCH", 5) == 0)
    {
        return 7;
    }
    else if (strncmp(request, "TRACE", 5) == 0)
    {
        return 8;
    }
    else if (strncmp(request, "CONNECT", 7) == 0)
    {
        return 9;
    }
    else if(strncmp(request, "TEST",4) == 0)
    {
        return 10;
    }
    else
    {
        return 0;
    }

}

void SendHttpResponse_Get(int sn)//发送HTTP响应函数
{
    printf("SenfHttpResponse_Get\r\n");
    const char httpResponse[] = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                                "<html>\r\n"
                                "<head>\r\n"
                                "<title>中核粒子医疗科技有限公司</title>\r\n"
                                "<style>\r\n"

                                "body { background-color: #FFFFFF }\r\n"
                                "h1\r\n"
                                "{\r\n"
                                "font-size:1cm;\r\n"
                                "text-align: center;\r\n"
                                "color: #0E3092;\r\n"
                                "}\r\n"
                                "h2{font-size:0.6cm;\r\n"
                                " text-align : center;\r\n"
                                "color: #0E3092;\r\n"
                                "}\r\n"
                                ".io-label {\r\n"
                                " margin-bottom: 10px;\r\n"
                                "}\r\n"
                                " .io-status {\r\n"
                                "display: inline-block;\r\n"
                                "width: 30px;\r\n"
                                "height: 30px;\r\n"
                                "border: 1px solid #ccc;\r\n"
                                "margin: 0 5px;\r\n"
                                "}\r\n"
                                ".io-on {\r\n"
                                "background-color: green;\r\n"
                                "}\r\n"
                                ".io-off {\r\n"
                                "background-color: red;\r\n"
                                "}\r\n"
                                "</style>\r\n"
                                "<script>\r\n"
                                "function updateIOStatus(io, status) {\r\n"
                                "var ioElement = document.getElementById('io' + io);\r\n"
                                "ioElement.classList.remove('io-on', 'io-off');\r\n"
                                "ioElement.classList.add(status ? 'io-on' : 'io-off');\r\n"
                                "}\r\n"
                                "function updateIOStatuses(statuses) {\r\n"
                                "for (var i = 0; i < statuses.length; i++) {\r\n"
                                "updateIOStatus(i + 1, statuses[i]);\r\n"
                                "}\r\n"
                                "}\r\n"
                                "function requestIOStatuses() {\r\n"
                                "var xhr = new XMLHttpRequest();\r\n"
                                "xhr.onreadystatechange = function () {\r\n"
                                "if (xhr.readyState === 4 && xhr.status === 200) {\r\n"
                                "var response = JSON.parse(xhr.responseText);\r\n"
                                "updateIOStatuses(response);\r\n"
                                "}\r\n"
                                "};\r\n"
                                "xhr.open('GET', '/io-status', true);\r\n"
                                "xhr.send();\r\n"
                                "}\r\n"
                                "setInterval(requestIOStatuses, 10);\r\n"
                                "</script>\r\n"
                                "</head>\r\n"
                                "<body>\r\n"
                                "<h1>中核粒子医疗科技有限公司</h1>\r\n"
                                "<!DOCTYPE html>\r\n"
                                "<p>LED 1 Switch</p>\r\n"
                                "<form method=\"post\">"
                                "<input type=\"hidden\" name=\"led\" value=\"1\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                "</form>\r\n"
                                "<p>LED 2 Switch</p>\r\n"
                                "<form method=\"post\">\r\n"
                                "<input type=\"hidden\" name=\"led\" value=\"2\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                "</form>"
                                //                                "<p>LED 3 Switch</p>\r\n"
                                //                                "<form method=\"post\">"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"3\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 4 Switch</p>\r\n"
                                //                                "<form method=\"post\">"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"4\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 5 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"5\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 6 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"6\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 7 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"7\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 8 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"8\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                "<div class=\"io-label\">\r\n"
                                "IO 1:\r\n"
                                "<div id=\"io1\" class=\"io-status\"></div>\r\n"
                                "</div>\r\n"
                                "<div class=\"io-label\">\r\n"
                                "IO 2:\r\n"
                                "<div id=\"io2\" class=\"io-status\"></div>\r\n"
                                "</div>\r\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 3:\n"
                                //                                "<div id=\"io3\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 4:\n"
                                //                                "<div id=\"io4\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 5:\n"
                                //                                "<div id=\"io5\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 6:\n"
                                //                                "<div id=\"io6\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 7:\n"
                                //                                "<div id=\"io7\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 8:\n"
                                //                                "<div id=\"io8\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                "</body>\r\n"
                                "</html>\r\n";

    send(sn, (uint8_t *) httpResponse, sizeof(httpResponse));
}


void HandleFormSubmissionLED(char *led, char *state)
{

    if (strcmp(led, "1") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(1);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(1);
        }
    }
    else if (strcmp(led, "2") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(2);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(2);
        }
    }
    else if (strcmp(led, "3") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(3);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(3);
        }
    }
    else if (strcmp(led, "4") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(4);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(4);
        }
    }
    else if (strcmp(led, "5") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(5);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(5);
        }
    }
    else if (strcmp(led, "6") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(6);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(6);
        }
    }
    else if (strcmp(led, "7") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(7);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(7);
        }
    }
    else if (strcmp(led, "8") == 0)
    {
        if (strcmp(state, "ON") == 0)
        {
            // 执行打开 LED 1 的操作
            led_on(8);
        }
        else if (strcmp(state, "OFF") == 0)
        {
            // 执行关闭 LED 1 的操作
            led_off(8);
        }
    }
}

/*
 * data_start = led=+1+&state=LED+1+On
value_start = +1+&state=LED+1+On
value_end = &state=LED+1+On

data_start = state=LED+1+On
value_start = LED+1+On
value_end =
led = LED+1+On
state = LED+1+On
 */
static char value_buffer[256];

char *get_form_data(char *request, char *keyword)
{
    memset(value_buffer,0xff,sizeof(value_buffer));
    char *data_start = strstr(request, keyword);  // 查找关键字在请求中的起始位置
    printf("data_start = %s\r\n", data_start);//led=+1+&state=LED+1+Off
    //static char value_buffer[256];  // 假设缓冲区大小为256
    if (data_start != NULL)
    {
        // 定位到关键字的值的起始位置
        char *value_start = strchr(data_start, '=') + 1;
        printf("value_start = %s\r\n", value_start);//data_start = led=+1+&state=LED+1+Off
        // 定位到关键字的值的结束位置（遇到'&'或'\0'）
        char *value_end = strchr(value_start, '&');
        if (value_end == NULL)
        {
            value_end = strchr(value_start, '\0');
        }
        printf("value_end = %s\r\n", value_end);
        // 计算关键字的值的长度（不包括字符串结束符）
        size_t value_length = value_end - value_start;
        printf("value_length = %zu\r\n", value_length);
        // 确保值的长度不超过缓冲区大小-1
        if (value_length >= sizeof(value_buffer))
        {
            value_length = sizeof(value_buffer) - 1;
        }

        // 复制关键字的值到静态缓冲区中，并添加字符串结束符
        strncpy(value_buffer, value_start, value_length);
        value_buffer[value_length] = '\0';
        return value_buffer;
    }
    else
    {
        // 如果找不到关键字，则返回空指针
        return NULL;
    }
}
int Get_IO_Flag(char *request)
{
    char *data_start = strstr(request, "io-status");
    if (data_start != NULL)
    {
        if (strncmp(data_start, "io", 2) == 0)
        {

            return 1;
        }
        else
        {

            return 0;
        }
    }
    else
    {
        return 0;
    }
}
/*
 * data_start = state=LED+1+On
value_start = LED+1+On
value_end =
value_length = 8
data_start = led=+1+&state=LED+1+On
value_start = +1+&state=LED+1+On
value_end = &state=LED+1+On
value_length = 3
led = +1+
state = +1+
 */
char *GetLED_Num(char *request)
{
//    pri//ntf("GetLED_Num = %s\r\n",request);
    return get_form_data(request, "led");
}

char *GetLED_States(char *request)
{
    // printf("GetLED_States = %s\r\n",request);
    return get_form_data(request, "state");
}
#if 0
void handle_request(char *request)
{
    char led_state[256];
    GetLED_States(gDATABUF);
    for (int i = 0; i < 256; i++)
    {
        led_state[i] = value_buffer[i];
    }
    printf("state = %s\r\n", led_state);
    char led_num[256];
    GetLED_Num(gDATABUF);
    for (int j = 0; j < 256; j++)
    {
        led_num[j] = value_buffer[j];
    }
    printf("led = %s\r\n", led_num);

    // 处理表单数
    HandleFormSubmissionLED(led_num, led_state);
    // 生成响应
    const char httpResponse[] = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                                "<html>\r\n"
                                "<head>\r\n"
                                "<title>中核粒子医疗科技有限公司</title>\r\n"
                                "<style>\r\n"

                                "body { background-color: #FFFFFF }\r\n"
                                "h1\r\n"
                                "{\r\n"
                                "font-size:1cm;\r\n"
                                "text-align: center;\r\n"
                                "color: #0E3092;\r\n"
                                "}\r\n"
                                "h2{font-size:0.6cm;\r\n"
                                " text-align : center;\r\n"
                                "color: #0E3092;\r\n"
                                "}\r\n"
                                ".io-label {\r\n"
                                " margin-bottom: 10px;\r\n"
                                "}\r\n"
                                " .io-status {\r\n"
                                "display: inline-block;\r\n"
                                "width: 30px;\r\n"
                                "height: 30px;\r\n"
                                "border: 1px solid #ccc;\r\n"
                                "margin: 0 5px;\r\n"
                                "}\r\n"
                                ".io-on {\r\n"
                                "background-color: green;\r\n"
                                "}\r\n"
                                ".io-off {\r\n"
                                "background-color: red;\r\n"
                                "}\r\n"
                                "</style>\r\n"
                                "<script>\r\n"
                                "function updateIOStatus(io, status) {\r\n"
                                "var ioElement = document.getElementById('io' + io);\r\n"
                                "ioElement.classList.remove('io-on', 'io-off');\r\n"
                                "ioElement.classList.add(status ? 'io-on' : 'io-off');\r\n"
                                "}\r\n"
                                "function updateIOStatuses(statuses) {\r\n"
                                "for (var i = 0; i < statuses.length; i++) {\r\n"
                                "updateIOStatus(i + 1, statuses[i]);\r\n"
                                "}\r\n"
                                "}\r\n"
                                "function requestIOStatuses() {\r\n"
                                "var xhr = new XMLHttpRequest();\r\n"
                                "xhr.onreadystatechange = function () {\r\n"
                                "if (xhr.readyState === 4 && xhr.status === 200) {\r\n"
                                "var response = JSON.parse(xhr.responseText);\r\n"
                                "updateIOStatuses(response);\r\n"
                                "}\r\n"
                                "};\r\n"
                                "xhr.open('GET', '/io-status', true);\r\n"
                                "xhr.send();\r\n"
                                "}\r\n"
                                "setInterval(requestIOStatuses, 10);\r\n"
                                "</script>\r\n"
                                "</head>\r\n"
                                "<body>\r\n"
                                "<h1>中核粒子医疗科技有限公司</h1>\r\n"
                                "<!DOCTYPE html>\r\n"
                                "<p>LED 1 Switch</p>\r\n"
                                "<form method=\"post\">"
                                "<input type=\"hidden\" name=\"led\" value=\"1\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                "</form>\r\n"
                                "<p>LED 2 Switch</p>\r\n"
                                "<form method=\"post\">\r\n"
                                "<input type=\"hidden\" name=\"led\" value=\"2\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                "</form>"
                                //                                "<p>LED 3 Switch</p>\r\n"
                                //                                "<form method=\"post\">"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"3\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 4 Switch</p>\r\n"
                                //                                "<form method=\"post\">"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"4\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 5 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"5\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 6 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"6\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 7 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"7\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                //                                "<p>LED 8 Switch</p>\r\n"
                                //                                "<form method=\"post\">\r\n"
                                //                                "<input type=\"hidden\" name=\"led\" value=\"8\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"ON\">\r\n"
                                //                                "<input type=\"submit\" name=\"state\" value=\"OFF\">\r\n"
                                //                                "</form>\r\n"
                                "<div class=\"io-label\">\r\n"
                                "IO 1:\r\n"
                                "<div id=\"io1\" class=\"io-status\"></div>\r\n"
                                "</div>\r\n"
                                "<div class=\"io-label\">\r\n"
                                "IO 2:\r\n"
                                "<div id=\"io2\" class=\"io-status\"></div>\r\n"
                                "</div>\r\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 3:\n"
                                //                                "<div id=\"io3\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 4:\n"
                                //                                "<div id=\"io4\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 5:\n"
                                //                                "<div id=\"io5\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 6:\n"
                                //                                "<div id=\"io6\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 7:\n"
                                //                                "<div id=\"io7\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                //                                "<div class=\"io-label\">\n"
                                //                                "IO 8:\n"
                                //                                "<div id=\"io8\" class=\"io-status\"></div>\n"
                                //                                "</div>\n"
                                "</body>\r\n"
                                "</html>\r\n";


    send(SOCK_TCPS, (uint8_t *) httpResponse, sizeof(httpResponse) - 1);
}
#endif
// led_on函数，点亮指定的LED
void led_on(uint8_t led_num)
{
    switch (led_num)
    {
        case 1:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
            break;
        case 4:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
            break;
        case 5:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);
            break;
        case 6:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);
            break;
        case 7:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);
            break;
        case 8:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
            break;

    }
}

// led_off函数，关闭指定的LED
void led_off(uint8_t led_num)
{
    switch (led_num)
    {
        case 1:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
            break;
        case 4:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
            break;
        case 5:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);
            break;
        case 6:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
            break;
        case 7:
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);
            break;
        case 8:
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
            break;

    }
}
void handleIOStatusRequest(uint8_t sn)
{
    uint8_t ioStatuses[2];
    // 获取IO状态并填充到ioStatuses数组中
    ioStatuses[0] = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8);
    ioStatuses[1] = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7);
    // 构建JSON格式的响应数据
    char jsonResponse[64]; // 根据实际情况调整大小
    snprintf(jsonResponse, sizeof(jsonResponse), "[%d, %d]", ioStatuses[0], ioStatuses[1]);

    // 构建HTTP响应头
    char httpHeader[128];
    snprintf(httpHeader, sizeof(httpHeader), "HTTP/1.1 200 OK\r\n"
                                             "Content-Type: application/json\r\n"
                                             "Content-Length: %d\r\n\r\n", strlen(jsonResponse));

    // 发送HTTP响应头
    send(sn, (uint8_t *)httpHeader, strlen(httpHeader));

    // 发送JSON响应体
    send(sn, (uint8_t *)jsonResponse, strlen(jsonResponse));
}
void http_String2Numbers(char *request)
{
//    int _numbers[MAX_HTTPSEND_NUMBERS];
    int _count = 0;
    //跳过响应头
    char* _start = strstr(request,"TEST / ");
    if(_start != NULL)
    {
//        printf("1\r\n");
        _start += strlen("TEST / ");
        char* token = strtok(_start," ");
        while((token != NULL)&&(_count <MAX_HTTPSEND_NUMBERS))
        {
            _numbers[_count++] =strtol(token,NULL,16);
            token = strtok(NULL," ");
        }
//        for(int i = 0; i < _count;i++)
//        {
//            printf("Number %d is %d\r\n",i,_numbers[i]);
//        }
    }
//    else
//    {
//        printf("2\r\n");
//    }

}
