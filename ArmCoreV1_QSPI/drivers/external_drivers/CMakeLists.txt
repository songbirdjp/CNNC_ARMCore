set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/backup_sram" 
    # "${current_path}/fram" 
    # "${current_path}/lan9252" 
    # "${current_path}/sdram" 
    # "${current_path}/w5500" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/backup_sram/*.*" 
    # "${current_path}/fram/*.*" 
    # "${current_path}/lan9252/*.*" 
    # "${current_path}/sdram/*.*" 
    # "${current_path}/w5500/*.*" 
    PARENT_SCOPE)