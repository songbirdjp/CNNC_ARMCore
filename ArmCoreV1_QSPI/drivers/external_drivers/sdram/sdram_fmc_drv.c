#include "fmc.h"
#include "sdram_fmc_drv.h"

uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;

    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;
    Command.CommandMode=cmd;                //命令
    Command.CommandTarget=target_bank;      //目标SDRAM存储区域
    Command.AutoRefreshNumber=refresh;      //自刷新次数
    Command.ModeRegisterDefinition=regval;  //要写入模式寄存器的值
    if(HAL_SDRAM_SendCommand(&hsdram1,&Command,0XFFFF)==HAL_OK) //向SDRAM发送命令
    {
        return 0;
    }
    else return 1;
}

//发送SDRAM初始化序列
void SDRAM_Init(void)
{
    uint32_t temp=0;
    //SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0);//时钟配置使能
    HAL_Delay(1);                                   //至少延时200us
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       //对所有存储区预充电
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//设置自刷新次数
    //配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
    //bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
    //bit9为指定的写突发模式，bit10和bit11位保留位
    
    temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |	//设置突发长度:1(可以是1/2/4/8)
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//设置突发类型:连续(可以是连续/交错)
         SDRAM_MODEREG_CAS_LATENCY_3           |	//设置CAS值:3(可以是2/3)
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //设置操作模式:0,标准模式
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //设置突发写模式:1,单点访问
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   //设置SDRAM的模式寄存器

    //刷新频率计数器(以SDCLK频率计数),计算方法:
    //COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
    //我们使用的SDRAM刷新周期为64ms,SDCLK=200/2=100Mhz,行数为8192(2^13).
    //所以,COUNT=64*1000*100/8192-20=761
    HAL_SDRAM_ProgramRefreshRate(&hsdram1,967);
}

//SDRAM内存测试
#define BLOCK 16*1024
void fsmc_sdram_test(void)
{
    uint32_t i=0;
    uint32_t temp=0;
    uint32_t sval=0;	//在地址0读到的数据

    //每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节
    for(i=0;i<32*1024*1024;i+=BLOCK)
    {
        *(__IO uint32_t*)(SDRAM_BANK1_ADDR+i)=temp;
        temp++;
    }
    //依次读出之前写入的数据,进行校验
    for(i=0;i<32*1024*1024;i+=BLOCK)
    {
        temp=*(__IO uint32_t*)(SDRAM_BANK1_ADDR+i);
        if(i==0)sval=temp;
        else if(temp<=sval) break;//后面读出的数据一定要比第一次读到的数据大.
        printf("SDRAM Capacity:%dKB\r\n",(uint16_t)(temp-sval+1)*16);//打印SDRAM容量
    }
}
