#include "stdlib.h"
#include "websocket.h"
#include "tcp_tasks.h"
#include <stdio.h>
#include "crypto_sha.h"

#ifdef IS_TCP_SERVER

SEND_INFO sendStructInfo;
static uint8_t serviceNumber = 0;
static CLIENT_INFO client[MAX_CLIENT_NUM];
static const char ws_base64char[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void ws_getRandomString(char *buff, uint32_t len)
{
    uint32_t i;
    uint8_t temp;
    //  srand((int32_t)time(0));
    srand(1);
    for (i = 0; i < len; i++)
    {
        temp = (uint8_t)(rand() % 256);
        if (temp == 0) // 随机数不要0
            temp = 128;
        buff[i] = temp;
    }
}

static int32_t ws_enPackage(
    uint8_t *data,
    uint32_t dataLen,
    uint8_t *package,
    uint32_t packageMaxLen,
    bool fin,
    bool mask,
    Ws_DataType type)
{
    uint32_t i, pkgLen = 0;
    // 掩码
    uint8_t maskKey[4] = {0};
    uint32_t maskCount = 0;
    // 最小长度检查
    if (packageMaxLen < 2)
        return -1;

    if(!fin)    type = WDT_MINDATA;//continuation frame
    // 根据包类型设置头字节
    if (type == WDT_MINDATA)
        *package++ = fin << 7;//0x80;
    else if (type == WDT_TXTDATA)
        *package++ = (fin << 7) + 1;//0x81;
    else if (type == WDT_BINDATA)
        *package++ = (fin << 7) + 2;//0x82;
    else if (type == WDT_DISCONN)
        *package++ = (fin << 7) + 8;//0x88;
    else if (type == WDT_PING)
        *package++ = (fin << 7) + 9;//0x89;
    else if (type == WDT_PONG)
        *package++ = (fin << 7) + 0xA;//0x8A;
    else
        return -1;
    pkgLen += 1;
    // 掩码位
    if (mask)
        *package = 0x80;
    // 半字节记录长度

    if (dataLen < 126)
    {
        *package++ |= (dataLen & 0x7F);
        pkgLen += 1;
    }
    // 2字节记录长度
    else if (dataLen < 65536)
    {
        if (packageMaxLen < 4)
            return -1;
        *package++ |= 0x7E;
        *package++ = (uint8_t)((dataLen >> 8) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 0) & 0xFF);
        pkgLen += 3;
    }
    // 8字节记录长度
    else
    {
        if (packageMaxLen < 10)
            return -1;
        *package++ |= 0x7F;
        *package++ = 0; // 数据长度变量是 uint32_t dataLen, 暂时没有那么多数据
        *package++ = 0;
        *package++ = 0;
        *package++ = 0;
        *package++ = (uint8_t)((dataLen >> 24) & 0xFF); // 到这里就够传4GB数据了
        *package++ = (uint8_t)((dataLen >> 16) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 8) & 0xFF);
        *package++ = (uint8_t)((dataLen >> 0) & 0xFF);
        pkgLen += 9;
    }
    // 数据使用掩码时,使用异或解码,maskKey[4]依次和数据异或运算,逻辑如下
    if (mask)
    {
        // 长度不足
        if (packageMaxLen < pkgLen + dataLen + 4)
            return -1;
        // 随机生成掩码
        ws_getRandomString((char *)maskKey, sizeof(maskKey));
        *package++ = maskKey[0];
        *package++ = maskKey[1];
        *package++ = maskKey[2];
        *package++ = maskKey[3];
        pkgLen += 4;
        for (i = 0, maskCount = 0; i < dataLen; i++, maskCount++)
        {
            // maskKey[4]循环使用
            if (maskCount == 4) // sizeof(maskKey))
                maskCount = 0;
            // 异或运算后得到数据
            *package++ = maskKey[maskCount] ^ data[i];
        }
        pkgLen += i;
        // 断尾
        *package = '\0';
    }
    // 数据没使用掩码, 直接复制数据段
    else
    {
        // 长度不足
        if (packageMaxLen < pkgLen + dataLen)
            return -1;
        // 这种方法,data指针位置相近时拷贝异常
        //  memcpy(package, data, dataLen);
        // 手动拷贝
        for (i = 0; i < dataLen; i++)
            *package++ = data[i];
        pkgLen += i;
        // 断尾
        *package = '\0';
    }

    return pkgLen;
}

static int32_t ws_dePackage(
    uint8_t *data,
    uint32_t len,
    uint32_t *retDataLen,
    uint32_t *retHeadLen,
    Ws_DataType *retPkgType)
{
    uint32_t cIn, cOut;
    // 包类型
    uint8_t type;
    // 数据段起始位置
    uint32_t dataOffset = 2;
    // 数据段长度
    uint32_t dataLen = 0;
    // 掩码
    uint8_t maskKey[4] = {0};
    bool mask = false;
    uint8_t maskCount = 0;
    // 数据长度过短
    if (len < 2)    {return 0;}
    // 解析包类型
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
        else    return 0;
    }
    else    return 0;
    
    // 是否掩码,及长度占用字节数
    if ((data[1] & 0x80) == 0x80)
    {
        mask = true;
        maskCount = 4;
    }
    // 2字节记录长度
    dataLen = data[1] & 0x7F;
    if (dataLen == 126)
    {
        // 数据长度不足以包含长度信息
        if (len < 4)    return 0;
        // 2字节记录长度
        dataLen = data[2];
        dataLen = (dataLen << 8) + data[3];
        // 转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 4 + maskCount;
        // 数据长度不足以包含掩码信息
        if (len < (uint32_t)(4 + maskCount))
            return -(int32_t)(4 + maskCount + dataLen - len);
        // 获得掩码
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
    // 8字节记录长度
    else if (dataLen == 127)
    {
        // 数据长度不足以包含长度信息
        if (len < 10)
            return 0;
        // 使用8个字节存储长度时,前4位必须为0,装不下那么多数据...
        if (data[2] != 0 || data[3] != 0 || data[4] != 0 || data[5] != 0)
            return 0;
        // 8字节记录长度
        dataLen = data[6];
        dataLen = (dataLen << 8) | data[7];
        dataLen = (dataLen << 8) | data[8];
        dataLen = (dataLen << 8) | data[9];
        // 转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 10 + maskCount;
        // 数据长度不足以包含掩码信息
        if (len < (uint32_t)(10 + maskCount))
            return -(int32_t)(10 + maskCount + dataLen - len);
        // 获得掩码
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
    // 半字节记录长度
    else
    {
        // 转储长度信息
        *retDataLen = dataLen;
        *retHeadLen = 2 + maskCount;
        // 数据长度不足
        if (len < (uint32_t)(2 + maskCount))
            return -(int32_t)(2 + maskCount + dataLen - len);
        // 获得掩码
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
    // 数据长度不足以包含完整数据段
    if (len < dataLen + dataOffset)
        return -(int32_t)(dataLen + dataOffset - len);
    // 解包数据使用掩码时, 使用异或解码, maskKey[4]依次和数据异或运算, 逻辑如下
 
    if (mask)
    {
        cIn = dataOffset;
        cOut = 0;
        maskCount = 0;
        // if (type == 0x02)   printf("bin");
        // else if (type == 0x01)  printf("txt");
        // printf(" %dB:", dataLen);
        for (; cOut < dataLen; cIn++, cOut++, maskCount++)
        {
            // maskKey[4]循环使用
            if (maskCount == 4) // sizeof(maskKey))
                maskCount = 0;
            // 异或运算后得到数据
            data[cOut] = maskKey[maskCount] ^ data[cIn];
         //   if (type == 0x02)   printf("%x ", data[cOut]);
          //  else if (type == 0x01)  printf("%c", data[cOut]);
        }
        // 断尾
        data[cOut] = '\0';
     //   printf("\r\n");
    }
    // 解包数据没使用掩码, 直接复制数据段
    else
    {
        // 这种方法,data指针位置相近时拷贝异常
        //  memcpy(data, &data[dataOffset], dataLen);
        // 手动拷贝
        cIn = dataOffset;
        cOut = 0;
        for (; cOut < dataLen; cIn++, cOut++)
            data[cOut] = data[cIn];
        // 断尾
        data[dataLen] = '\0';
    }
    // 有些特殊包数据段长度可能为0,这里为区分格式错误返回,置为1
    if (dataLen == 0)   dataLen = 1;

    return dataLen;
}

int32_t ws_send(uint8_t s, void *buff, int32_t buffLen, bool fin, bool mask, Ws_DataType type)
{
	uint8_t headLen = 8;					
    uint8_t wsPkg[buffLen + headLen];
    int32_t retLen, ret;
    // 参数检查
	if(client[s].connectStatus < 1) return 0; //handshake failed!
    if ((buffLen < 0) || ((buffLen + headLen) > 2048))
        return -1;
    // 非包数据发送
    if (type == WDT_NULL)
    {
        printf("send WDT_NULL \r\n");
        //  memcpy(wsPkg, (uint8_t *)buff, buffLen);
        //  for(int i = 0; i < buffLen; i++)    printf("%x\r\n", wsPkg[i]);
        return tcp_client_data_send(s, (uint8_t *)buff, buffLen);
    }

    // 数据打包 +14 预留类型、掩码、长度保存位
    memset(wsPkg, 0, buffLen + headLen);
    retLen = ws_enPackage((uint8_t *)buff, buffLen, wsPkg, (buffLen + headLen), fin, mask, type);
    //   printf("retLen: %d\r\n",retLen );
    if (retLen <= 0)    return -1;
    
    // 显示数据
  //   printf("ws_send: %x %x %x %x %x %x\r\n", wsPkg[0],wsPkg[1],wsPkg[2],wsPkg[3],wsPkg[4],wsPkg[5] );
    //  for(int32_t i = 0; i < retLen; i++)    printf("0x%x ", wsPkg[i]);
    ret = tcp_client_data_send(s, wsPkg, retLen);

    return ret;
}

int32_t ws_recv(uint8_t s, void* buff, int32_t buffSize, Ws_DataType* retType, uint32_t* retHeadLen)
{
    int32_t retDePkg = -1;        //调用解包的返回
    uint32_t timeout = 0;    //接收超时计数
    Ws_DataType retPkgType = WDT_NULL; //默认返回包类型
    uint8_t maxHeadLen = 14;
    uint32_t retDataLen = 0; //解包得到的数据段长度
    int32_t retFinal = -1;

    retDePkg = ws_dePackage((uint8_t*)buff, maxHeadLen, &retDataLen, retHeadLen, &retPkgType); //为防止一次接收到多包数据(粘包),先尝试性解析ws头部字节（最大14Bytes）,得知总长度后再解析剩下部分
    if (retDePkg == 0 || (retDePkg < 0 && maxHeadLen - retDePkg > buffSize))//头部解析失败或解析出的包长过大（超过2K）
    {
         printf("tcp recv error %d\r\n", retDePkg);
    } 
    else
    {
       // printf("retDePkg1 %d\r\n", retDePkg);
        if (retDePkg < 0)//尝试解析出完整的一包
        {
            retDePkg = ws_dePackage((uint8_t*)buff, maxHeadLen - retDePkg, &retDataLen, retHeadLen, &retPkgType);
        }
      //  printf("retDePkg2 %d\r\n", *retHeadLen);
        if (retDePkg > 0)   //已经解析到完整的一包
        {
            //收到 PING 包,应自动回复 PONG
            if (retPkgType == WDT_PING)
            {
                //自动 ping-pong
              //  ws_send(s, NULL, 0, true, false, WDT_PONG);
                // WS_INFO("ws_recv: WDT_PING\r\n");
                retFinal = 0;
            }
            //收到 PONG 包
            else if (retPkgType == WDT_PONG)
            {
                // WS_INFO("ws_recv: WDT_PONG\r\n");
                retFinal = 0;
            }
            //收到 断连 包
            else if (retPkgType == WDT_DISCONN)
            {
                // WS_INFO("ws_recv: WDT_DISCONN\r\n");
                retFinal = 0;
            }
            //其它正常数据包
            else    retFinal = retDePkg;
        }
        else    retFinal = retDePkg; //没有解析到完整的一包，说明本次接收的数据不包含完整的数据段
    } 

    if (retType)    *retType = retPkgType;
 
    return retFinal; 
}

static void sha1_hash(const char *source, char *buff)
{
    uint8_t Message_Digest[64], i;
    uint32_t len;
    
   if(crypto_sha1_cal(source, strlen(source), Message_Digest, &len))
        printf("SHA1 ERROR: Could not compute message digest \r\n");
    else
    {
        for(i = 0; i < len; i++)    sprintf(buff+2*i, "%02X", Message_Digest[i]);
        
    }
}

static int32_t ws_base64_encode(const uint8_t *bindata, char *base64, int32_t binlength)
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

static int32_t ws_buildRespondShakeKey(char *acceptKey, uint32_t acceptKeyLen, char *respondKey)
{
    char sha1DataTemp[128] = {0};
    uint8_t sha1Data[65] = {0}; // 128/2+1
    int32_t i, j, ret;
    const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    uint32_t guidLen = sizeof(guid), sha1DataTempLen;
    char clientKey[acceptKeyLen + guidLen + 10];

    if (acceptKey == NULL)
        return 0;

    memcpy(clientKey, acceptKey, acceptKeyLen);
    memcpy(&clientKey[acceptKeyLen], guid, guidLen);

   // printf("message: %s\r\n", clientKey);
     sha1_hash(clientKey, sha1DataTemp);
     sha1DataTempLen = strlen((const char *)sha1DataTemp);
    // printf("digest:  %d %s\r\n", sha1DataTempLen, sha1DataTemp);

    // 把hex字符串如"12ABCDEF",转为数值数组如{0x12,0xAB,0xCD,0xEF}
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

    ret = ws_base64_encode((const uint8_t *)sha1Data, (char *)respondKey, j);

    return ret;
}

static void ws_buildHttpRespond(char *acceptKey, uint32_t acceptKeyLen, char *package)
{
    const char httpResp[] =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";

    char respondShakeKey[256] = {0};
    // 构建回应的握手key
    ws_buildRespondShakeKey(acceptKey, acceptKeyLen, respondShakeKey);
    // 组成回复信息
    sprintf(package, httpResp, respondShakeKey);
}

static int32_t ws_replyClient(uint8_t s, char *buff, char *path)
{
    char *keyOffset;
    uint32_t ret;
    char recvShakeKey[30] = {0};
    char respondPackage[256] = {0};

    //  printf("%s\r\n",buff);
    // path检查
    if (path && !strstr((char *)buff, path))
    {
        printf("path not matched\r\n");
        return -1;
    }
    // 获取握手key
    if (!(keyOffset = strstr((char *)buff, "Sec-WebSocket-Key: ")))
    {
        printf("Sec-WebSocket-Key not found\r\n");
        return -1;
    }
    // 获取握手key
    keyOffset += strlen("Sec-WebSocket-Key: ");
    sscanf((const char *)keyOffset, "%s", recvShakeKey);
    ret = strlen((const char *)recvShakeKey);
  //  printf("recvShakeKey len %d\r\n", ret);
    if (ret < 1)
    {
        printf("Sec-WebSocket-Key not matched\r\n");
        return -1;
    }
    // 创建回复key
    ws_buildHttpRespond(recvShakeKey, ret, respondPackage);
  //    printf("response %s\r\n",respondPackage);
    tcp_client_data_send(s, (uint8_t *)respondPackage, strlen(respondPackage));
    printf("Handshake Success!\r\n");

    return 1;
}

int8_t getClientType(uint8_t s, uint8_t *pString)
{
    char *p = strstr(pString, "authorization=");

    if (p != NULL)
    {
        if(strstr(p, CONTROLLER_AUTHORIZATION)) 
        {
            client[s].clientType = CONTROLLER;//this client is controller
            printf("client %d is controller\r\n", s);
        }
        else if( strstr(p, SERVICE_AUTHORIZATION) )
        {
            client[s].clientType = SERVICE;//this client is service
            printf("client %d is service\r\n", s);
            serviceNumber++;
        }
        else    return -1;
    }
    else    return -1;

    return 1;
}

bool operateSendMutex(bool opType, uint8_t itemIndex, uint32_t timeout)   // opType = 1:Acquire opType = 0:Release
{
    if(sendStructInfo.pActiveSend[itemIndex].sendUpdateMutexHandle == NULL) return 0;

    if(opType)  osMutexAcquire(sendStructInfo.pActiveSend[itemIndex].sendUpdateMutexHandle, timeout);
    else    osMutexRelease(sendStructInfo.pActiveSend[itemIndex].sendUpdateMutexHandle);

    return 1;
}

bool isSendPeriod(uint8_t sn, uint8_t itemIndex) // to inquire if current loop is sending loop for a group of period send data, if yes, you can do sth. in this loop before or after send
{
    if((sendStructInfo.pActiveSend == NULL) || (sendStructInfo.sendItemNum == 0))   return 0;// no data to send    
    if((client[sn].loopCnt % sendStructInfo.pActiveSend[itemIndex].controlSignal) == 0)   return 1;
    
    return 0;
}

uint8_t isClientTypeMatch(uint8_t sn, uint8_t itemIndex)// to inquire if assigned client type of a group of data match with the real client(sn), if true, return the type
{
    if(client[sn].clientType == -1) return 0;//no client
    if((sendStructInfo.pActiveSend == NULL) || (sendStructInfo.sendItemNum == 0))   return 0;// no data to send    

    if(((client[sn].clientType == SERVICE) && (sendStructInfo.pActiveSend[itemIndex].assignedClientType == SERVICE)) || 
        ((client[sn].clientType == CONTROLLER) && (sendStructInfo.pActiveSend[itemIndex].assignedClientType == CONTROLLER)) ||
        (sendStructInfo.pActiveSend[itemIndex].assignedClientType == ALL_CLIENTS))
        return sendStructInfo.pActiveSend[itemIndex].assignedClientType; //match

    return 0;//not match
}

int32_t tcp_recv_process(TCP_DATA_t *recvData)
{
    uint8_t s = recvData->sn;
    uint8_t *data = recvData->gDATABUF;
    uint16_t len = recvData->Len, i;
    Ws_DataType retPkgType = WDT_NULL;
    int32_t ret = 0;
    APP_DATA_RECV itemRecv;
    uint32_t retHeadLen = 0;

   // printf("recv length = %d\r\n", len);
    
    if (strncmp(data, "GET", 3) == 0)
    { // deal with handshake
        // printf("%s\r\n",data);
        if (strstr(data, "Sec-WebSocket-Key"))
        {
            ret = ws_replyClient(s, data, "/");
            if (ret > 0)
            {
                ret = getClientType(s, data);
                if(ret < 0){
                    printf("failed to get client type!\r\n"); 
                }
                else{
                    client[s].socketNum = s;
                    client[s].loopCnt = 0;
                    client[s].connectStatus = ret; 
                }  
            } 
            else{
                printf("handshake failed!\r\n");
                ret = -1;
            }   
        }
        else{
            printf("Invalid client request!\r\n");
            ret = -1;
        }
        #if 0
        else{
                printf("show service interface!\r\n");
                showServiceInterfaceOnPC(s);
               // disconnect(s);
                close(s);
                client[s].clientType = 1;//service, default is 0 - developer
        }
        #endif
    }
    else if (client[s].connectStatus > 0)
    {   // recv data after handshake
       // for (i = 0; i < len; i++) printf("%x ", data[i]);
       // printf("\r\n");
    
        ret = ws_recv(s, data, DATA_BUF_SIZE, &retPkgType, &retHeadLen);
//printf("ret %d\r\n", retHeadLen);
        if(ret < 0)//本包数据内容或长度错误，直接丢弃
        {
            printf("this pack is wrong\r\n");
            return 0;
        } 
        else//正常解包
        {
            switch(retPkgType)
            {
            case WDT_DISCONN:
                printf("socket%d going to disconnect!\r\n", s);
                uint8_t closeFrame[] = {0x88, 0x02, 0x03, 0xe8}; // status code:1000    close normal
                ws_send(s, closeFrame, sizeof(closeFrame), true, false, WDT_NULL);
                close(s);
                client[s].connectStatus = 0;
                client[s].clientType = 0;
                client[s].socketNum = -1;
				client[s].loopCnt = 0;	
                if(client[s].clientType == SERVICE) serviceNumber--;
                break;	
            case WDT_PING:	
                uint16_t payloadLen = len - retHeadLen;
                uint8_t *pdata = NULL;
                 printf("recv ping and reply pong %d\r\n",payloadLen);
                //  for(i=0; i< payloadLen; i++)
                //     printf("%x ",data[i]);
                // printf("\r\n");
                if(payloadLen > 0)  pdata = data;
                ws_send(s, pdata, payloadLen, true, false, WDT_PONG);
                break;	 
            case WDT_TXTDATA:
               // printf("recv data: %s\r\n",data);s
            case WDT_BINDATA:
                itemRecv.length = len - retHeadLen;
                itemRecv.sn = s;
                itemRecv.recvDataType = retPkgType;
                itemRecv.clientType = client[s].clientType;
                itemRecv.tcpData = data;
            //    for(uint8_t i = 0; i < retS.retDataLen; i++) printf("%d ", data[retS.retHeadLen+i]);
              //  printf("\r\n");
                nrtRecvDataProcess(&itemRecv);
            break;
            default:    break;
            }
        }
    }

    return ret;
}

int32_t tcp_send_process(uint8_t s)
{
    uint8_t i;
    int32_t ret = 0;

    if ((s > MAX_CLIENT_NUM) || (s < 0))
    {
        printf("Invalid socket number %d!\r\n", s);
        return -1;
    }

    if ((client[s].socketNum == -1) || (client[s].connectStatus < 1))
    {
      //  printf("Not a client sn %d!\r\n", s);
        return 0;//sn not bind with a client yet
    }

    if((sendStructInfo.pActiveSend == NULL) || (sendStructInfo.sendItemNum == 0))
    {
      //  printf("no send data struct is attached\r\n");
        return 0;// no data to send
    } 

    for (i = 0; i < sendStructInfo.sendItemNum; i++)
    {
        if(sendStructInfo.pActiveSend[i].tcpData == NULL)   continue;//no data is assigned, go to next item
        if (((sendStructInfo.pActiveSend[i].controlSignal == TO_SEND) || ((sendStructInfo.pActiveSend[i].controlSignal > STOP_SEND) && isSendPeriod(s, i)))//send once or send period 
            && (isClientTypeMatch(s, i) > 0))//client type match
        {   // send data
            if((sendStructInfo.pActiveSend[i].sendMode == WDT_BINDATA) || (sendStructInfo.pActiveSend[i].sendMode == WDT_TXTDATA) || (sendStructInfo.pActiveSend[i].sendMode == WDT_NULL))
            {
                if(!operateSendMutex(1, i, osWaitForever))  printf("%s mutex is NULL!\r\n", sendStructInfo.pActiveSend[i].name);
                ret = ws_send(s, sendStructInfo.pActiveSend[i].tcpData, sendStructInfo.pActiveSend[i].length, true, false, sendStructInfo.pActiveSend[i].sendMode);
                operateSendMutex(0, i, 0);
            }
            else{
                printf("Wrong ws send data type!\r\n");
                return -1;
            } 

            if(ret > 0){    //send success
                if(sendStructInfo.pActiveSend[i].controlSignal == TO_SEND)
                {
                    if(client[s].clientType == CONTROLLER)  sendStructInfo.pActiveSend[i].controlSignal = STOP_SEND;//only one controller
                    else if(client[s].clientType == SERVICE)//wait for all service send once finish, then clean flag
                    {
                        if( ++sendStructInfo.pActiveSend[i].onceSendCnt >= serviceNumber ){
                            sendStructInfo.pActiveSend[i].controlSignal = STOP_SEND;
                            sendStructInfo.pActiveSend[i].onceSendCnt = 0;
                        }
                    }    
                }    
            }  
            else{
                printf("ws send failed!\r\n");
                return -1;
            }   
        }
    } 
    client[s].loopCnt++; 
   
    return ret;
}

void tcp_server_init(void)
{
    for (uint8_t i = 0; i < MAX_CLIENT_NUM; i++)
    {
        client[i].socketNum = -1;
        client[i].clientType = -1;
    }
}

#endif