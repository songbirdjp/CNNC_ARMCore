#include "hw_sys_state.h"
#include "stm32h7xx_hal.h"
#include "hw_bkp_reg.h"
#include "shell.h"

__weak int8_t system_power_on_post_process(void)
{
    return 0;
}

__weak int8_t system_reboot_beforehand(void)
{
    return 0;
}


int8_t system_reboot(void)
{
    /* 1. deal with something if user needed */
    system_reboot_beforehand();

    /* 2. save reboot flag */
    bkp_reg_write(REG_REBOOT_FLAG, 1);

    /* 3. hot reset */
    HAL_NVIC_SystemReset();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(system_reboot, system_reboot, reboot system);

int8_t system_reset_status_check(void)
{
    int8_t ret = 0;
    uint32_t reboot_flag = 0;

    ret = bkp_reg_read(REG_REBOOT_FLAG, &reboot_flag);
    if (ret != 0)
    {
        printf("read reboot flag err: %d\r\n", ret);
        return ret;
    }
    else
    {
        if (reboot_flag == 1)
        {
            bkp_reg_write(REG_REBOOT_FLAG, 0);
            printf("system rebooted by user\r\n");
            return 0;
        }
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_CPURST) != 0)
    {
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != 0)
        {
            __HAL_RCC_CLEAR_RESET_FLAGS();

            system_power_on_post_process();
        }
        else
        {
            printf("CPU reset checked\r\n");
            printf("RCC_FLAG_D1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_D1RST));
            printf("RCC_FLAG_D2RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_D2RST));
            printf("RCC_FLAG_BORRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST));
            printf("RCC_FLAG_PINRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST));
            printf("RCC_FLAG_PORRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST));
            printf("RCC_FLAG_SFTRST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST));
            printf("RCC_FLAG_IWDG1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST));
            printf("RCC_FLAG_WWDG1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST));
            printf("RCC_FLAG_LPWR1RST: %d\r\n", __HAL_RCC_GET_FLAG(RCC_FLAG_LPWR1RST));
        }
    }

    return 0;
}