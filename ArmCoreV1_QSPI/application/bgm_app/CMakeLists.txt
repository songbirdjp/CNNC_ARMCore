set(current_path ${CMAKE_CURRENT_LIST_DIR})

set(include_path ${include_path} 
    "${current_path}/adc_app"
    "${current_path}/afc_app" 
    "${current_path}/dose_app" 
    "${current_path}/eps_app" 
    "${current_path}/vps_app" 
    "${current_path}/io_app" 
    "${current_path}/rtm_app" 
    "${current_path}/interlock_app" 
    "${current_path}" 
    PARENT_SCOPE)

set(src_path ${src_path}
    "${current_path}/adc_app/*.*"
    "${current_path}/afc_app/*.*" 
    "${current_path}/dose_app/*.*" 
    "${current_path}/eps_app/*.*" 
    "${current_path}/vps_app/*.*" 
    "${current_path}/io_app/*.*" 
    "${current_path}/rtm_app/*.*" 
    "${current_path}/interlock_app/*.*" 
    "${current_path}/*.*" 
    PARENT_SCOPE)