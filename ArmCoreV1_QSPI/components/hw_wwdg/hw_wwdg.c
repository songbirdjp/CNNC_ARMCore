#include "hw_wwdg.h"
#include "wwdg.h"
#include "init_call.h"

void wwdg_refresh(void)
{
    // HAL_WWDG_RegisterCallback

    WWDG_HandleTypeDef hwwdg1;

    hwwdg1.Instance = WWDG1;
    hwwdg1.Init.Prescaler = WWDG_PRESCALER_128;
    hwwdg1.Init.Window = 0x7F;
    hwwdg1.Init.Counter = 0x7F;
    hwwdg1.Init.EWIMode = WWDG_EWI_ENABLE;

    HAL_StatusTypeDef state = HAL_WWDG_Refresh(&hwwdg1);
    if (state != HAL_OK)
    {
        Error_Handler();
    }
}

int8_t wwdg_init(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    MX_WWDG1_Init();

    status = HAL_WWDG_RegisterCallback(&hwwdg1, HAL_WWDG_EWI_CB_ID, wwdg_refresh);
    if (status != HAL_OK)
    {
        printf("Error registering WWDG callback\r\n");
    }

    return 0;
}
INIT_BOARD_EXPORT(wwdg_init);
