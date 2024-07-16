#include <stddef.h>
#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "string.h"
#include "stdlib.h"
#include "tcp_server.h"
#include "tcp_client.h"

static CLIENT_INFO client[MAX_CLIENT_NUM];
static DATA_TEST testStruct = {1,1,180,4,0,"1_2_3"};
void dealWithRecvData(void);

static TCP_DATA_ITEMS itemSendByControl = {
        "toEPIDParams",sizeof(testStruct),2,0,3,0,&testStruct,NULL
};

static TCP_DATA_ITEMS itemSendPeriodic[] = {
        "testSend",sizeof(testStruct),1,2,3,0,&testStruct,NULL
};

static TCP_DATA_ITEMS itemRecv = {
        "testRecv", 0,2,0,3,0,&testStruct,dealWithRecvData
};

#define SHA1CircularShift(bits, word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32 - (bits))))
static const char ws_base64char[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void dealWithRecvData(void)
{
    if(itemRecv.status == 1){

    }
    //do sth. to process recv data
    osDelay(100);
    itemRecv.status = 2;//note that data has used
}

int8_t do_tcp_server_send(uint8_t s)
{
    int8_t ret = 0;

     switch (getSn_SR(s))
    {
        case SOCK_INIT:
            listen(s);
        //    if(s==1) printf("SERVER_SOCK_INIT\r\n");
            break;
        case SOCK_ESTABLISHED:
            tcp_establish_cb(); //period feedback here
            break;
        case SOCK_CLOSE_WAIT:
            osDelay(500);
            close(s);
            break;
        case SOCK_CLOSED:
            ret = socket(s, Sn_MR_TCP, 80, 0);
            break;
        default:break;
    }

    return ret;
}

static void ws_getRandomString(char* buff, uint32_t len)
{
    uint32_t i;
    uint8_t temp;
  //  srand((int32_t)time(0));
    srand(1);
    for (i = 0; i < len; i++)
    {
        temp = (uint8_t)(rand() % 256);
        if (temp == 0) //随机数不要0
            temp = 128;
        buff[i] = temp;
    }
}

static int32_t ws_enPackage(
        uint8_t* data,
        uint32_t dataLen,
        uint8_t* package,
        uint32_t packageMaxLen,
        bool mask,
        Ws_DataType type)
{
    uint32_t i, pkgLen = 0;
    //掩码
    uint8_t maskKey[4] = {0};
    uint32_t maskCount = 0;
    //最小长度检查
    if (packageMaxLen < 2)
        return -1;
    //根据包类型设置头字节
    if (type == WDT_MINDATA)
        *package++ = 0x80;
    else if (type == WDT_TXTDATA)
        *package++ = 0x81;
    else if (type == WDT_BINDATA)
        *package++ = 0x82;
    else if (type == WDT_DISCONN)
        *package++ = 0x88;
    else if (type == WDT_PING)
        *package++ = 0x89;
    else if (type == WDT_PONG)
        *package++ = 0x8A;
    else
        return -1;
    pkgLen += 1;
    //掩码位
    if (mask)
        *package = 0x80;
    //半字节记录长度

    if (dataLen < 126)
    {
        *package++ |= (dataLen & 0x7F);
        pkgLen += 1;
    }
        //2字节记录长度
    else if (dataLen < 65536)
    {
        if (packageMaxLen < 4)
            return -1;
        *package++ |= 0x7E;
        *package++ = (uint8_t)((dataLen >> 8) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 0) & 0xFF);
        pkgLen += 3;
    }
        //8字节记录长度
    else
    {
        if (packageMaxLen < 10)
            return -1;
        *package++ |= 0x7F;
        *package++ = 0; //数据长度变量是 uint32_t dataLen, 暂时没有那么多数据
        *package++ = 0;
        *package++ = 0;
        *package++ = 0;
        *package++ = (uint8_t)((dataLen >> 24) & 0xFF); //到这里就够传4GB数据了
        *package++ = (uint8_t)((dataLen >> 16) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 8) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 0) & 0xFF);
        pkgLen += 9;
    }
    //数据使用掩码时,使用异或解码,maskKey[4]依次和数据异或运算,逻辑如下
    if (mask)
    {
        //长度不足
        if (packageMaxLen < pkgLen + dataLen + 4)
            return -1;
        //随机生成掩码
        ws_getRandomString((char*)maskKey, sizeof(maskKey));
        *package++ = maskKey[0];
        *package++ = maskKey[1];
        *package++ = maskKey[2];
        *package++ = maskKey[3];
        pkgLen += 4;
        for (i = 0, maskCount = 0; i < dataLen; i++, maskCount++)
        {
            //maskKey[4]循环使用
            if (maskCount == 4) //sizeof(maskKey))
                maskCount = 0;
            //异或运算后得到数据
            *package++ = maskKey[maskCount] ^ data[i];
        }
        pkgLen += i;
        //断尾
        *package = '\0';
    }
        //数据没使用掩码, 直接复制数据段
    else
    {
        //长度不足
        if (packageMaxLen < pkgLen + dataLen)
            return -1;
        //这种方法,data指针位置相近时拷贝异常
        // memcpy(package, data, dataLen);
        //手动拷贝
        for (i = 0; i < dataLen; i++)
            *package++ = data[i];
        pkgLen += i;
        //断尾
        *package = '\0';
    }

    return pkgLen;
}

static int32_t ws_dePackage(
        uint8_t* data,
        uint32_t len,
        uint32_t* retDataLen,
        uint32_t* retHeadLen,
        Ws_DataType* retPkgType)
{
    uint32_t cIn, cOut;
    //包类型
    uint8_t type;
    //数据段起始位置
    uint32_t dataOffset = 2;
    //数据段长度
    uint32_t dataLen = 0;
    //掩码
    uint8_t maskKey[4] = {0};
    bool mask = false;
    uint8_t maskCount = 0;
    //数据长度过短
    if (len < 2)
        return 0;
    //解析包类型
    if ((data[0] & 0x80) == 0x80)
    {
        type = data[0] & 0x0F;
        if (type == 0x00)
            *retPkgType = WDT_MINDATA;
        else if (type == 0x01)
            *retPkgType = WDT_TXTDATA;
        else if (type == 0x02)
            *retPkgType = WDT_BINDATA;
        else if (type == 0x08)
            *retPkgType = WDT_DISCONN;
        else if (type == 0x09)
            *retPkgType = WDT_PING;
        else if (type == 0x0A)
            *retPkgType = WDT_PONG;
        else
            return 0;
    }
    else
        return 0;
    //是否掩码,及长度占用字节数
    if ((data[1] & 0x80) == 0x80)
    {
        mask = true;
        maskCount = 4;
    }
    //2字节记录长度
    dataLen = data[1] & 0x7F;
    if (dataLen == 126)
    {
        //数据长度不足以包含长度信息
        if (len < 4)
            return 0;
        //2字节记录长度
        dataLen = data[2];
        dataLen = (dataLen << 8) + data[3];
        //转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 4 + maskCount;
        //数据长度不足以包含掩码信息
        if (len < (uint32_t)(4 + maskCount))
            return -(int32_t)(4 + maskCount + dataLen - len);
        //获得掩码
        if (mask)
        {
            maskKey[0] = data[4];
            maskKey[1] = data[5];
            maskKey[2] = data[6];
            maskKey[3] = data[7];
            dataOffset = 8;
        }
        else
            dataOffset = 4;
    }
        //8字节记录长度
    else if (dataLen == 127)
    {
        //数据长度不足以包含长度信息
        if (len < 10)
            return 0;
        //使用8个字节存储长度时,前4位必须为0,装不下那么多数据...
        if (data[2] != 0 || data[3] != 0 || data[4] != 0 || data[5] != 0)
            return 0;
        //8字节记录长度
        dataLen = data[6];
        dataLen = (dataLen << 8) | data[7];
        dataLen = (dataLen << 8) | data[8];
        dataLen = (dataLen << 8) | data[9];
        //转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 10 + maskCount;
        //数据长度不足以包含掩码信息
        if (len < (uint32_t)(10 + maskCount))
            return -(int32_t)(10 + maskCount + dataLen - len);
        //获得掩码
        if (mask)
        {
            maskKey[0] = data[10];
            maskKey[1] = data[11];
            maskKey[2] = data[12];
            maskKey[3] = data[13];
            dataOffset = 14;
        }
        else
            dataOffset = 10;
    }
        //半字节记录长度
    else
    {
        //转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 2 + maskCount;
        //数据长度不足
        if (len < (uint32_t)(2 + maskCount))
            return -(int32_t)(2 + maskCount + dataLen - len);
        //获得掩码
        if (mask)
        {
            maskKey[0] = data[2];
            maskKey[1] = data[3];
            maskKey[2] = data[4];
            maskKey[3] = data[5];
            dataOffset = 6;
        }
        else
            dataOffset = 2;
    }
    //数据长度不足以包含完整数据段
    if (len < dataLen + dataOffset)
        return -(int32_t)(dataLen + dataOffset - len);
    //解包数据使用掩码时, 使用异或解码, maskKey[4]依次和数据异或运算, 逻辑如下
    if (mask)
    {
        cIn = dataOffset;
        cOut = 0;
        maskCount = 0;
        printf("recv data %d: ", dataLen);
        for (; cOut < dataLen; cIn++, cOut++, maskCount++)
        {
            //maskKey[4]循环使用
            if (maskCount == 4) //sizeof(maskKey))
                maskCount = 0;
            //异或运算后得到数据
            data[cOut] = maskKey[maskCount] ^ data[cIn];
            printf("%c",  data[cOut]);
        }
        //断尾
        data[cOut] = '\0';
        printf("\r\n");
    }
        //解包数据没使用掩码, 直接复制数据段
    else
    {
        //这种方法,data指针位置相近时拷贝异常
        // memcpy(data, &data[dataOffset], dataLen);
        //手动拷贝
        cIn = dataOffset;
        cOut = 0;
        for (; cOut < dataLen; cIn++, cOut++)
            data[cOut] = data[cIn];
        //断尾
        data[dataLen] = '\0';
    }
    //有些特殊包数据段长度可能为0,这里为区分格式错误返回,置为1
    if (dataLen == 0)
        dataLen = 1;
    return dataLen;
}

int32_t ws_send(uint8_t s, void* buff, int32_t buffLen, bool mask, Ws_DataType type)
{
   // uint8_t* wsPkg = NULL;
    uint8_t wsPkg[buffLen + 14];
    int32_t retLen, ret;
    //参数检查
    if (buffLen < 0)
        return 0;
    //非包数据发送
    if (type == WDT_NULL){
        printf("send WDT_NULL \r\n");
      //  memcpy(wsPkg, (uint8_t *)buff, buffLen);
      //  for(int i = 0; i < buffLen; i++)    printf("%x\r\n", wsPkg[i]);
        return tcp_client_data_send(s, (uint8_t *) buff, buffLen);
      // return 0;
    }

       // return send(fd, buff, buffLen, MSG_NOSIGNAL);
    //数据打包 +14 预留类型、掩码、长度保存位
   // wsPkg = (uint8_t*)pvPortMalloc(buffLen + 14);
    memset(wsPkg, 0, buffLen + 14);
    retLen = ws_enPackage((uint8_t*)buff, buffLen, wsPkg, (buffLen + 14), mask, type);
 //   printf("retLen: %d\r\n",retLen );
    if (retLen <= 0)
    {
      //  vPortFree(wsPkg);
        return 0;
    }

    //显示数据
    //printf("ws_send: %x %x %x %x %x %x\r\n", wsPkg[0],wsPkg[1],wsPkg[2],wsPkg[3],wsPkg[4],wsPkg[5] );
  //  for(int32_t i = 0; i < retLen; i++)    printf("0x%x ", wsPkg[i]);
//
   // ret = send(fd, wsPkg, retLen, MSG_NOSIGNAL);
    ret = tcp_client_data_send(s, wsPkg, retLen);
  //  vPortFree(wsPkg);
    return ret;
}

static void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const uint32_t K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
    int32_t t;
    uint32_t temp;
    uint32_t W[80];
    uint32_t A, B, C, D, E;

    for (t = 0; t < 16; t++)
    {
        W[t] = ((uint32_t)context->Message_Block[t * 4]) << 24;
        W[t] |= ((uint32_t)context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((uint32_t)context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((uint32_t)context->Message_Block[t * 4 + 3]);
    }

    for (t = 16; t < 80; t++)
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }
    context->Message_Digest[0] = (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] = (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] = (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] = (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] = (context->Message_Digest[4] + E) & 0xFFFFFFFF;
    context->Message_Block_Index = 0;
}

static void SHA1Reset(SHA1Context* context)
{
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;

    context->Message_Digest[0] = 0x67452301;
    context->Message_Digest[1] = 0xEFCDAB89;
    context->Message_Digest[2] = 0x98BADCFE;
    context->Message_Digest[3] = 0x10325476;
    context->Message_Digest[4] = 0xC3D2E1F0;

    context->Computed = 0;
    context->Corrupted = 0;
}

static void SHA1PadMessage(SHA1Context* context)
{
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64)
            context->Message_Block[context->Message_Block_Index++] = 0;
        SHA1ProcessMessageBlock(context);
        while (context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 56)
            context->Message_Block[context->Message_Block_Index++] = 0;
    }
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessageBlock(context);
}

static int32_t SHA1Result(SHA1Context* context)
{
    if (context->Corrupted)
    {
        return 0;
    }
    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
    }
    return 1;
}

static void SHA1Input(SHA1Context* context, const char* message_array, uint32_t length)
{
    if (!length)
        return;

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while (length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);

        context->Length_Low += 8;

        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
                context->Corrupted = 1;
        }

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }
        message_array++;
    }
}

//static char* sha1_hash(const char* source, char* buff)
static void sha1_hash(const char* source, char* buff)
{
    SHA1Context sha;
  //  char* buff = NULL;

    SHA1Reset(&sha);
    SHA1Input(&sha, source, strlen(source));

    if (!SHA1Result(&sha))
        printf("SHA1 ERROR: Could not compute message digest \r\n");
    else
    {
       // buff = (char*)pvPortMalloc(128);
        sprintf(buff, "%08lX%08lX%08lX%08lX%08lX",
                sha.Message_Digest[0],
                sha.Message_Digest[1],
                sha.Message_Digest[2],
                sha.Message_Digest[3],
                sha.Message_Digest[4]);
    }
  //  return buff;
}

int32_t ws_base64_encode(const uint8_t* bindata, char* base64, int32_t binlength)
{
    int32_t i, j;
    uint8_t current;
    for (i = 0, j = 0; i < binlength; i += 3)
    {
        current = (bindata[i] >> 2);
        current &= (uint8_t)0x3F;
        base64[j++] = ws_base64char[(int32_t)current];
        current = ((uint8_t)(bindata[i] << 4)) & ((uint8_t)0x30);
        if (i + 1 >= binlength)
        {
            base64[j++] = ws_base64char[(int32_t)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ((uint8_t)(bindata[i + 1] >> 4)) & ((uint8_t)0x0F);
        base64[j++] = ws_base64char[(int32_t)current];
        current = ((uint8_t)(bindata[i + 1] << 2)) & ((uint8_t)0x3C);
        if (i + 2 >= binlength)
        {
            base64[j++] = ws_base64char[(int32_t)current];
            base64[j++] = '=';
            break;
        }
        current |= ((uint8_t)(bindata[i + 2] >> 6)) & ((uint8_t)0x03);
        base64[j++] = ws_base64char[(int32_t)current];
        current = ((uint8_t)bindata[i + 2]) & ((uint8_t)0x3F);
        base64[j++] = ws_base64char[(int32_t)current];
    }
    base64[j] = '\0';
    return j;
}

static int32_t ws_buildRespondShakeKey(char* acceptKey, uint32_t acceptKeyLen, char* respondKey)
{
   // char* clientKey;
    //  char* sha1DataTemp;
     // uint8_t* sha1Data;
    char sha1DataTemp[128]={0};
    uint8_t sha1Data[65]={0}; // 128/2+1
    int32_t i, j, ret;
    const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    uint32_t guidLen = sizeof(guid) , sha1DataTempLen;
    char clientKey[acceptKeyLen + guidLen + 10];

    if (acceptKey == NULL)
        return 0;

   // guidLen = sizeof(guid);
   // clientKey = (char*)pvPortMalloc(acceptKeyLen + guidLen + 10);
    memcpy(clientKey, acceptKey, acceptKeyLen);
    memcpy(&clientKey[acceptKeyLen], guid, guidLen);

    sha1_hash(clientKey, sha1DataTemp);
    sha1DataTempLen = strlen((const char*)sha1DataTemp);
   // sha1Data = (uint8_t*)pvPortMalloc(sha1DataTempLen / 2 + 1);
  
    //把hex字符串如"12ABCDEF",转为数值数组如{0x12,0xAB,0xCD,0xEF}
    for (i = j = 0; i < sha1DataTempLen;)
    {
        if (sha1DataTemp[i] > '9')
            sha1Data[j] = (10 + sha1DataTemp[i] - 'A') << 4;
        else
            sha1Data[j] = (sha1DataTemp[i] - '0') << 4;

        i += 1;

        if (sha1DataTemp[i] > '9')
            sha1Data[j] |= (10 + sha1DataTemp[i] - 'A');
        else
            sha1Data[j] |= (sha1DataTemp[i] - '0');

        i += 1;
        j += 1;
    }

    ret = ws_base64_encode((const uint8_t*)sha1Data, (char*)respondKey, j);

    // vPortFree(sha1DataTemp);
    // vPortFree(sha1Data);
    // vPortFree(clientKey);
    return ret;
}

static void ws_buildHttpRespond(char* acceptKey, uint32_t acceptKeyLen, char* package)
{
    const char httpResp[] =
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "Sec-WebSocket-Version: 13\r\n\r\n";

    char respondShakeKey[256] = {0};
    //构建回应的握手key
    ws_buildRespondShakeKey(acceptKey, acceptKeyLen, respondShakeKey);
    //组成回复信息
    sprintf(package, httpResp, respondShakeKey);
}

int32_t ws_replyClient(uint8_t s, char* buff, char* path)
{
    char* keyOffset;
    uint32_t ret;
    char recvShakeKey[100] = {0};
    char respondPackage[256] = {0};

  //  printf("%s\r\n",buff);
    //path检查
    if (path && !strstr((char*)buff, path))
    {
        printf("path not matched\r\n");
        return -1;
    }
    //获取握手key
    if (!(keyOffset = strstr((char*)buff, "Sec-WebSocket-Key: ")))
    {
        printf("Sec-WebSocket-Key not found\r\n");
        return -1;
    }
    //获取握手key
    keyOffset += strlen("Sec-WebSocket-Key: ");
    sscanf((const char*)keyOffset, "%s", recvShakeKey);
    ret = strlen((const char*)recvShakeKey);
    if (ret < 1)
    {
        printf("Sec-WebSocket-Key not matched\r\n");
        return -1;
    }
    //创建回复key
    ws_buildHttpRespond(recvShakeKey, ret, respondPackage);
  //  printf("response %s\r\n",respondPackage);
  //  send(s, (uint8_t *) respondPackage, sizeof(respondPackage));
    tcp_client_data_send(s, (uint8_t *) respondPackage, sizeof(respondPackage));
    printf("Handshake Success!\r\n");

    return 1;
}

#define CONTROLLER_IP   "192.168.10.100"
void tcp_recv_process(TCP_DATA_t *recvData)
{
    uint32_t retDataLen = 0; //解包得到的数据段长度
    uint32_t retHeadLen = 0; //解包得到的包头部长度
    Ws_DataType retPkgType = WDT_NULL; //默认返回包类型
    uint8_t s = recvData->sn;
    char *data = (char*)recvData->gDATABUF;
    uint16_t len = recvData->Len;

    if (strncmp(data, "GET", 3) == 0){// deal with handshake
        if(strstr(data, "Sec-WebSocket-Key")){
          //  printf("%s\r\n",data);
            client[s].socketNum = s;
            getSn_DIPR(s, client[s].destIP);
            char chDestIP[20];
            sprintf(chDestIP, "%d.%d.%d.%d", client[s].destIP[0],client[s].destIP[1],client[s].destIP[2],client[s].destIP[3]);
            if(strncmp(chDestIP, CONTROLLER_IP, sizeof(CONTROLLER_IP))==0)  client[s].clientType = 0;
            else    client[s].clientType = 1;
          //  printf("%s %d\r\n", chDestIP, client[s].clientType);
         //   printf("destIP: %d:%d:%d:%d\r\n", client[s].destIP[0], client[s].destIP[1],client[s].destIP[2],client[s].destIP[3]);
            client[s].connectStatus = ws_replyClient(s,data, "/");
            if(client[s].connectStatus > 0) client[s].loopCnt = 0;
        }
        // else{
        //         printf("show service interface!\r\n");
        //         showServiceInterfaceOnPC(s);
        //         disconnect(s);
        //         client[s].clientType = 1;//service, default is 0 - developer
        // }
    }
    else if(client[s].connectStatus > 0) {// recv data after handshake
                   // for (i = 0; i < len; i++) printf("%x ", recvInfo.gDATABUF[i]);
        ws_dePackage(data, len, &retDataLen, &retHeadLen, &retPkgType);
        if (retPkgType == WDT_DISCONN) {
            printf("socket%d going to disconnect!\r\n",s);
            uint8_t closeFrame[] = {0x88, 0x02, 0x03, 0xe8};//status code:1000    close normal
            ws_send(s,closeFrame, sizeof(closeFrame), false, WDT_NULL);
          //  disconnect(s);
            close(s);
            client[s].connectStatus = 0;
            client[s].clientType = 0;
        }
        else if(WDT_TXTDATA == retPkgType){
            printf("recv text\r\n");

        }
        else if(WDT_BINDATA == retPkgType){
            printf("recv binary\r\n");
            itemRecv.status = 1; //note that data has updated
            memcpy(itemRecv.tcpData, data, len);
            if(itemRecv.cbFunc != NULL)   itemRecv.cbFunc(); //  deal with receive data in callback function
        }
    }
}

static uint16_t sendPeriodNum, sendByControlNum;
void tcp_send_process(void)
{
    uint8_t s, i;

   for(s=0; s<MAX_CLIENT_NUM; s++ ){
        if((client[s].socketNum >= 0) && ( client[s].connectStatus > 0 )){
            for(i = 0; i < sendByControlNum; i++){
                if(itemSendByControl.status == 1){
                    if(client[s].clientType > 0){   //send data to service 

                    }
                    else{ 
                        if(itemSendByControl.sendMode == WDT_BINDATA)
                            ws_send(s,itemSendByControl.tcpData, itemSendByControl.Length, false, WDT_BINDATA);
                        else if(itemSendByControl.sendMode == WDT_TXTDATA)
                             ws_send(s,itemSendByControl.tcpData, itemSendByControl.Length, false, WDT_TXTDATA);
                        if(itemSendByControl.cbFunc != NULL)   itemSendByControl.cbFunc();
                        itemSendByControl.status = 2;
                    }
                }
            }
            for(i = 0; i < sendPeriodNum; i++){
                if((itemSendPeriodic[i].sendPeriod > 0)&&((client[s].loopCnt++ % itemSendPeriodic[i].sendPeriod)==0)){
                    if(client[s].clientType > 0){   //send data to service 

                    }
                    else{   //send data to program
                         //  printf("send %d!!!\r\n", loop_cnt);
                        uint8_t temp[]={1,2,3,4};
                        //   char temp[]="1234";
                        itemSendPeriodic[i].tcpData=temp;
                        itemSendPeriodic[i].Length = 4;
                        if(itemSendByControl.sendMode == WDT_BINDATA)
                            ws_send(s,itemSendPeriodic[i].tcpData,itemSendPeriodic[i].Length,false,WDT_BINDATA);
                        else if(itemSendByControl.sendMode == WDT_TXTDATA)
                            ws_send(s,itemSendPeriodic[i].tcpData,itemSendPeriodic[i].Length,false,WDT_TXTDATA);
                        if(itemSendPeriodic[i].cbFunc != NULL)   itemSendPeriodic[i].cbFunc();
                    //   ws_send(itemSendPeriodic[i].tcpData,itemSendPeriodic[i].Length,false,WDT_TXTDATA);
                    }      
                }
            }   
        }
   }
}

void tcp_app_init(void)
{
    for(uint8_t i=0; i<MAX_CLIENT_NUM; i++ ){
        client[i].socketNum = -1;
        client[i].clientType = -1;
   }
   sendPeriodNum = sizeof(itemSendPeriodic)/sizeof(TCP_DATA_ITEMS);
   sendByControlNum = sizeof(itemSendByControl)/sizeof(TCP_DATA_ITEMS);

}