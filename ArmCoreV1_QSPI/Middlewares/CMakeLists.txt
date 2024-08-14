set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" 
    "${current_path}/Third_Party/FreeRTOS/Source/include" 
    "${current_path}/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" 
    "${current_path}/ST/STM32_Cryptographic/include" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/*.*" 
    PARENT_SCOPE)

set(link_path ${link_path}
    "${current_path}/ST/STM32_Cryptographic/lib/libSTM32Cryptographic_CM7.a"
    PARENT_SCOPE)