#include "hw_ramecc.h"
#include "ramecc.h"
#include "init_call.h"

static void (*init_fun_cb[RAM_ECC_MAX])(void *arg) = {NULL};
static void (*double_err_cb[RAM_ECC_MAX])(void *arg) = {NULL};

static int8_t ram_init(void)
{
    struct ram_info
    {
        uint32_t addr_base;
        uint32_t size;
    };

    struct ram_info info[RAM_ECC_MAX] = {
        {0x24000000, 0}, /* AXI SRAM */
        {0x00000000, 0x10000}, /* ITCM */
        {0x20000000, 0x10000}, /* D0TCM */
        {0x20010000, 0x10000}, /* D1TCM */
        {0x24020000, 0}, /* AXI Shared */
        {0x30000000, 0x4000},  /* SRAM1 */
        {0x30004000, 0x4000},  /* SRAM2 */
        {0x4000A000, 0},  /* FDCAN SRAM */  /* TODO：地址未知 */
        {0x38000000, 0x4000},  /* SRAM4 */
        {0x38800000, 0},  /* Backup SRAM */
    };

    for (uint8_t i = 0; i < RAM_ECC_MAX; i++)
    {
        if (info[i].size == 0)
        {
            continue;
        }

        if (init_fun_cb[i] != NULL)
        {
            init_fun_cb[i](&info[i]);
        }
        else
        {
            memset((uint32_t *)info[i].addr_base, 0, info[i].size);
        }
    }

    return 0;
}
static int8_t hw_ram_ecc_init(void)
{
    int8_t ret = 0;
    HAL_StatusTypeDef stat = HAL_OK;

    /* 1. disable ram ecc */
    stat |= HAL_RAMECC_DeInit(&hramecc1_m1);
    stat |= HAL_RAMECC_DeInit(&hramecc1_m2);
    stat |= HAL_RAMECC_DeInit(&hramecc1_m3);
    stat |= HAL_RAMECC_DeInit(&hramecc1_m4);
    stat |= HAL_RAMECC_DeInit(&hramecc1_m6);
    stat |= HAL_RAMECC_DeInit(&hramecc2_m1);
    stat |= HAL_RAMECC_DeInit(&hramecc2_m2);
    stat |= HAL_RAMECC_DeInit(&hramecc2_m3);
    stat |= HAL_RAMECC_DeInit(&hramecc3_m1);
    stat |= HAL_RAMECC_DeInit(&hramecc3_m2);
    if (stat != HAL_OK)
    {
        printf("ram ecc deinit err: %d\r\n", stat);
        return -1;
    }

    /* 2. ram initialization */
    ret = ram_init();
    if (ret != 0)
    {
        printf("ram init err: %d\r\n", ret);
        return -2;
    }

    /* 3. ram ecc init */
    MX_RAMECC_Init();

    /* 4. enable ram ecc interrupt */
    stat |= HAL_RAMECC_EnableNotification(&hramecc1_m1, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc1_m2, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc1_m3, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc1_m4, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc1_m6, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc2_m1, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc2_m2, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc2_m3, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc3_m1, RAMECC_IT_MONITOR_ALL);
    stat |= HAL_RAMECC_EnableNotification(&hramecc3_m2, RAMECC_IT_MONITOR_ALL);
    if (stat != HAL_OK)
    {
        printf("ram ecc init err: %d\r\n", stat);
        return -3;
    }

    /* 5. start ram ecc monitor */
    stat |= HAL_RAMECC_StartMonitor(&hramecc1_m1);
    stat |= HAL_RAMECC_StartMonitor(&hramecc1_m2);
    stat |= HAL_RAMECC_StartMonitor(&hramecc1_m3);
    stat |= HAL_RAMECC_StartMonitor(&hramecc1_m4);
    stat |= HAL_RAMECC_StartMonitor(&hramecc1_m6);
    stat |= HAL_RAMECC_StartMonitor(&hramecc2_m1);
    stat |= HAL_RAMECC_StartMonitor(&hramecc2_m2);
    stat |= HAL_RAMECC_StartMonitor(&hramecc2_m3);
    stat |= HAL_RAMECC_StartMonitor(&hramecc3_m1);
    stat |= HAL_RAMECC_StartMonitor(&hramecc3_m2);
    if (stat != HAL_OK)
    {
        printf("ram ecc start err: %d\r\n", stat);
        return -4;
    }

    /* 6. enable nvic interrupt */
    HAL_NVIC_SetPriority(ECC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ECC_IRQn);

    return 0;
}
INIT_BOARD_EXPORT(hw_ram_ecc_init);

static RAMECC_HandleTypeDef *hw_ram_ecc_get(enum ram_ecc_type ram_ecc)
{
    RAMECC_HandleTypeDef *hramecc = NULL;

    switch (ram_ecc)
    {
    case RAM_ECC_AXI:
        hramecc = &hramecc1_m1;
        break;
    case RAM_ECC_ITCM:
        hramecc = &hramecc1_m2;
        break;
    case RAM_ECC_D0TCM:
        hramecc = &hramecc1_m3;
        break;
    case RAM_ECC_D1TCM:
        hramecc = &hramecc1_m4;
        break;
    case RAM_ECC_AXI_SHARED:
        hramecc = &hramecc1_m6;
        break;
    case RAM_ECC_SRAM1:
        hramecc = &hramecc2_m1;
        break;
    case RAM_ECC_SRAM2:
        hramecc = &hramecc2_m2;
        break;
    case RAM_ECC_FDCAN:
        hramecc = &hramecc2_m3;
        break;
    case RAM_ECC_SRAM4:
        hramecc = &hramecc3_m1;
        break;
    case RAM_ECC_BKP:
        hramecc = &hramecc3_m2;
        break;
    default:
        break;
    }

    return hramecc;
}
static enum ram_ecc_type hw_ram_ecc_type_get(RAMECC_HandleTypeDef *hramecc)
{
    if (hramecc == &hramecc1_m1)
    {
        return RAM_ECC_AXI;
    }
    else if (hramecc == &hramecc1_m2)
    {
        return RAM_ECC_ITCM;
    }
    else if (hramecc == &hramecc1_m3)
    {
        return RAM_ECC_D0TCM;
    }
    else if (hramecc == &hramecc1_m4)
    {
        return RAM_ECC_D1TCM;
    }
    else if (hramecc == &hramecc1_m6)
    {
        return RAM_ECC_AXI_SHARED;
    }
    else if (hramecc == &hramecc2_m1)
    {
        return RAM_ECC_SRAM1;
    }
    else if (hramecc == &hramecc2_m2)
    {
        return RAM_ECC_SRAM2;
    }
    else if (hramecc == &hramecc2_m3)
    {
        return RAM_ECC_FDCAN;
    }
    else if (hramecc == &hramecc3_m1)
    {
        return RAM_ECC_SRAM4;
    }
    else if (hramecc == &hramecc3_m2)
    {
        return RAM_ECC_BKP;
    }

    return RAM_ECC_MAX;
}
static uint32_t hw_ram_ecc_failing_address_abs_get(enum ram_ecc_type ram_ecc)
{
    uint32_t base_addr = 0, bus_width = 0, addr = 0;

    addr = HAL_RAMECC_GetFailingAddress(hw_ram_ecc_get(ram_ecc));

    switch (ram_ecc)
    {
    case RAM_ECC_AXI:
        base_addr = 0x24000000;
        bus_width = 8;
        break;
    case RAM_ECC_ITCM:
        base_addr = 0x00000000;
        bus_width = 8;
        break;
    case RAM_ECC_D0TCM:
        base_addr = 0x20000000;
        bus_width = 4;
        break;
    case RAM_ECC_D1TCM:
        base_addr = 0x20000004;
        bus_width = 4;
        break;
    case RAM_ECC_AXI_SHARED:
        break;
    case RAM_ECC_SRAM1:
        base_addr = 0x30000000;
        bus_width = 4;
        break;
    case RAM_ECC_SRAM2:
        base_addr = 0x30004000;
        bus_width = 4;
        break;
    case RAM_ECC_FDCAN:
        break;
    case RAM_ECC_SRAM4:
        base_addr = 0x38000000;
        bus_width = 4;
        break;
    case RAM_ECC_BKP:
        base_addr = 0x38800000;
        bus_width = 4;
        break;
    default:
        break;
    }

    return (base_addr + addr * bus_width);
}
void HAL_RAMECC_DetectErrorCallback(RAMECC_HandleTypeDef *hramecc)
{
    uint32_t err_code = HAL_RAMECC_GetRAMECCError(hramecc);

    if (err_code != HAL_RAMECC_NO_ERROR)
    {
        hramecc->RAMECCErrorCode = HAL_RAMECC_NO_ERROR;

        enum ram_ecc_type ram_ecc = hw_ram_ecc_type_get(hramecc);
        if (ram_ecc == RAM_ECC_MAX)
        {
            printf("ram ecc type err\r\n");
            return;
        }

        uint32_t err_addr = hw_ram_ecc_failing_address_abs_get(ram_ecc);
        printf("err addr: %#.8x\r\n", err_addr);

        if (err_code & HAL_RAMECC_SINGLEERROR_DETECTED != 0)
        {
            printf("ram ecc single error detected: %d\r\n", ram_ecc);
            // printf("error address: %#.8x\r\n", err_addr);
            // printf("error data low: %#.8x\r\n", HAL_RAMECC_GetFailingDataLow(hramecc));
            // printf("error data high: %#.8x\r\n", HAL_RAMECC_GetFailingDataHigh(hramecc));
            // printf("error hamming code: %#.8x\r\n", HAL_RAMECC_GetHammingErrorCode(hramecc));
            // printf("error code: %d\r\n", HAL_RAMECC_GetError(hramecc));
            // printf("ram ecc state: %d\r\n", HAL_RAMECC_GetState(hramecc));

            uint32_t data = *(uint32_t *)err_addr;  /* read and write back */
            *(uint32_t *)err_addr = data;
        }
        if (err_code & HAL_RAMECC_DOUBLEERROR_DETECTED != 0)
        {
            printf("ram ecc double error detected: %d\r\n", ram_ecc);
            // printf("error address: %#.8x\r\n", hw_ram_ecc_failing_address_abs_get(ram_ecc));
            // printf("error data low: %#.8x\r\n", HAL_RAMECC_GetFailingDataLow(hramecc));
            // printf("error data high: %#.8x\r\n", HAL_RAMECC_GetFailingDataHigh(hramecc));
            // printf("error hamming code: %#.8x\r\n", HAL_RAMECC_GetHammingErrorCode(hramecc));
            // printf("error code: %d\r\n", HAL_RAMECC_GetError(hramecc));
            // printf("ram ecc state: %d\r\n", HAL_RAMECC_GetState(hramecc));

            if (double_err_cb[ram_ecc] != NULL)
            {
                double_err_cb[ram_ecc](&ram_ecc);
            }
        }
    }
}

int8_t hw_ram_ecc_init_func_callback_register(enum ram_ecc_type ram_ecc, int8_t (*callback)(void *arg))
{
    if (ram_ecc >= RAM_ECC_MAX || ram_ecc < 0)
    {
        return -1;
    }

    init_fun_cb[ram_ecc] = callback;

    return 0;
}
int8_t hw_ram_ecc_double_err_callback_register(enum ram_ecc_type ram_ecc, int8_t (*callback)(void *arg))
{
    if (ram_ecc >= RAM_ECC_MAX || ram_ecc < 0)
    {
        return -1;
    }

    double_err_cb[ram_ecc] = callback;

    return 0;
}


#ifdef RAMECC_TEST
static int8_t hw_ram_ecc_init_callback(void *arg)
{
    memset((uint32_t *)0x00000004, 0, 0x10000 - 4);

    return 0;
}
static int8_t hw_ram_ecc_double_err_callback(void *arg)
{
    printf("ram ecc double error detected\r\n");

    return 0;
}
static int8_t hw_ram_ecc_callback_register(void)
{
    hw_ram_ecc_init_func_callback_register(RAM_ECC_ITCM, hw_ram_ecc_init_callback);
    hw_ram_ecc_double_err_callback_register(RAM_ECC_ITCM, hw_ram_ecc_double_err_callback);

    return 0;
}
// INIT_ENV_EXPORT(hw_ram_ecc_callback_register);
#endif