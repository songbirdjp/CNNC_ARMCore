set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path} 
    "${current_path}/board_para" 
    "${current_path}/bootloader_patch" 
    "${current_path}/freertos" 
    "${current_path}/main" 
    "${current_path}/main_app" 
    "${current_path}/sys_cfg" 
    "${current_path}/timestamp" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/board_para/*.*" 
    "${current_path}/bootloader_patch/*.*" 
    "${current_path}/freertos/*.*" 
    "${current_path}/main/*.*" 
    "${current_path}/main_app/*.*" 
    "${current_path}/sys_cfg/*.*" 
    "${current_path}/timestamp/*.*" 
    PARENT_SCOPE)