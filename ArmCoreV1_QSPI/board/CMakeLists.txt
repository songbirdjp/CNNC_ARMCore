set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/board_config/inc" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/board_config/*.*" 
    "${current_path}/linker_scripts/*.*" 
    "${current_path}/startup/*.*" 
    PARENT_SCOPE)