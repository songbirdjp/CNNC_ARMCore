set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/cjson" 
    "${current_path}/cm_backtrace" 
    "${current_path}/common" 
    "${current_path}/console" 
    "${current_path}/crypto" 
    "${current_path}/gpio" 
    "${current_path}/gpio_imitate" 
    "${current_path}/hw_crc" 
    "${current_path}/hw_semaphore" 
    "${current_path}/hw_wwdg" 
    "${current_path}/pid_ctrl" 
    "${current_path}/shell" 
    "${current_path}/uart_frame" 
    "${current_path}/ulog" 
    "${current_path}/utilities" 
    "${current_path}/ymodem" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/cjson/*.*" 
    "${current_path}/cm_backtrace/*.*" 
    "${current_path}/common/*.*" 
    "${current_path}/console/*.*" 
    "${current_path}/crypto/*.*" 
    "${current_path}/gpio/*.*" 
    "${current_path}/gpio_imitate/*.*" 
    "${current_path}/hw_crc/*.*" 
    "${current_path}/hw_semaphore/*.*" 
    "${current_path}/hw_wwdg/*.*" 
    "${current_path}/pid_ctrl/*.*" 
    "${current_path}/shell/*.*" 
    "${current_path}/uart_frame/*.*" 
    "${current_path}/ulog/*.*" 
    "${current_path}/utilities/*.*" 
    "${current_path}/ymodem/*.*" 
    PARENT_SCOPE)