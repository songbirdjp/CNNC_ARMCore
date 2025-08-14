set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/CMSIS/Device/ST/STM32H7xx/Include" 
    "${current_path}/CMSIS/Include" 
    "${current_path}/CMSIS_DSP/Include" 
    "${current_path}/STM32H7xx_HAL_Driver/Inc" 
    "${current_path}/STM32H7xx_HAL_Driver/Inc/Legacy" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/*.*" 
    PARENT_SCOPE)

set(link_path ${link_path}
    "${current_path}/CMSIS_DSP/Lib/libarm_cortexM7lfsp_math.a"
    PARENT_SCOPE)