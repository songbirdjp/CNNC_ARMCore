set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/*.*" 
    PARENT_SCOPE)