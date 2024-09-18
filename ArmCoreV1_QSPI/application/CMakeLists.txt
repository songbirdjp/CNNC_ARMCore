add_subdirectory(app)

set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path}
    "${current_path}/adc" 
    "${current_path}/adcs7476" 
    # "${current_path}/ethercat" 
    # "${current_path}/fdcan" 
    "${current_path}/flash" 
    # "${current_path}/fpga" 
    "${current_path}/ltc2632" 
    "${current_path}/system_common" 
    # "${current_path}/tcp" 
    "${current_path}/uart" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/adc/*.*" 
    "${current_path}/adcs7476/*.*"  
    # "${current_path}/ethercat/*.*" 
    # "${current_path}/fdcan/*.*" 
    "${current_path}/flash/*.*" 
    # "${current_path}/fpga/*.*" 
    "${current_path}/ltc2632/*.*" 
    "${current_path}/system_common/*.*" 
    # "${current_path}/tcp/*.*" 
    "${current_path}/uart/*.*" 
    PARENT_SCOPE)