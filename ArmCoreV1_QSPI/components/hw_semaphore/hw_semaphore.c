#include "hw_semaphore.h"
#include "init_call.h"
#include "stm32h7xx_hal.h"

typedef void (*HSEM_Callback)(uint32_t SemMask);

static HSEM_Callback hsem_callback[HSEM_SEMID_MAX] = {0};

static int8_t hsem_clk_enable(void)
{
    __HAL_RCC_HSEM_CLK_ENABLE();

    return 0;
}
INIT_BOARD_EXPORT(hsem_clk_enable);

void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
    printf("--HAL_HSEM_FreeCallback--%u\r\n",SemMask);

    uint8_t bit_index = 0;

    while(SemMask)
    {
        bit_index = POSITION_VAL(SemMask);

        // printf("bit_index:%d\r\n", bit_index);

        if (hsem_callback[bit_index] != NULL)
        {
            hsem_callback[bit_index](SemMask);
        }

        SemMask -= (1 << bit_index);
    }
}

int8_t hsem_irq_enable_set(uint8_t hsem_id, uint8_t enable)
{
    if (!IS_HSEM_SEMID(hsem_id))
    {
        return -1;
    }

    enable == 0 ? HAL_HSEM_DeactivateNotification(__HAL_HSEM_SEMID_TO_MASK(hsem_id)) : HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(hsem_id));

    return 0;
}

int8_t hsem_lock_take(uint8_t hsem_id, uint8_t process_id)
{
    if (!IS_HSEM_SEMID(hsem_id))
    {
        return -1;
    }

    if (IS_HSEM_PROCESSID(process_id))
    {
        return -2;
    }

    if(HAL_HSEM_Take(hsem_id, process_id) != HAL_OK)
    {
        printf("HSEM %u %u take failed\r\n", hsem_id, process_id);
        return -3;
    }

    return 0;
}

int8_t hsem_lock_release(uint8_t hsem_id, uint8_t process_id)
{
    if (!IS_HSEM_SEMID(hsem_id))
    {
        return -1;
    }

    if (IS_HSEM_PROCESSID(process_id))
    {
        return -2;
    }

    HAL_HSEM_Release(hsem_id, process_id);

    return 0;
}

int8_t hsem_lock_release_irq_callback_register(uint8_t hsem_id, void (*callback)(uint32_t SemMask))
{
    if (!IS_HSEM_SEMID(hsem_id))
    {
        return -1;
    }

    hsem_callback[hsem_id] = callback;

    return 0;
}