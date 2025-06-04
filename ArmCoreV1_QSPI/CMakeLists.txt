#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.25)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(ETHERCAT_CNNCPM C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv5-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv5-d16)
set(COMMON_FLAGS "-specs=nosys.specs -specs=nano.specs -u _printf_float -u _scanf_float")

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include(config.cmake)
set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
message(STATUS "git branch: ${GIT_BRANCH}")

set(GIT_HASH "")
get_git_hash(GIT_HASH)
message(STATUS "git hash: ${GIT_HASH}")

configure_file(config.h.in "../application/app/sys_cfg/config.h")

set(include_path "")
set(src_path "")
set(link_path "")
add_subdirectory(Middlewares)
add_subdirectory(libraries)
add_subdirectory(drivers)
add_subdirectory(components)
add_subdirectory(board)
add_subdirectory(application)
add_subdirectory(bootloader)

# message("include_path: ${include_path}")
# message("src_path: ${src_path}")
# message("link_path: ${link_path}")

include_directories( ${include_path} )

file(GLOB_RECURSE SOURCES ${src_path})

# remove fdcan driver source files
# file(GLOB_RECURSE DEL_SOURCES 
#     "drivers/internal_drivers/fdcan/*.*")

# list(REMOVE_ITEM SOURCES ${DEL_SOURCES})

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/board/linker_scripts/STM32H723ZGTX_FLASH.ld)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf ${link_path})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(INFO_BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_INFO.bin)
set(MAKE_FILE ${PROJECT_BINARY_DIR}/Makefile)
set(EC_FILE ${CMAKE_SOURCE_DIR}/drivers/external_drivers/lan9252/lan9252_appObjects.h)
set(SYS_CFG_FILE ${CMAKE_SOURCE_DIR}/application/app/sys_cfg/sys_cfg.c)

cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
message(STATUS "os: ${OS_NAME}")

if(OS_NAME MATCHES "Linux")
    message(STATUS "Running on Linux")
elseif(OS_NAME MATCHES "Windows")
    message(STATUS "Running on Windows")
elseif(OS_NAME MATCHES "Darwin")
    message(STATUS "Running on macOS")
else()
    message(STATUS "Running on another OS: ${OS_NAME}")
endif()

set(CMAKE_USER ${CMAKE_SOURCE_DIR}/info_generation)
set(CMAKE_USER_MAKEFILE ${CMAKE_SOURCE_DIR}/makefile_update)
set(CMAKE_USER_EC ${CMAKE_SOURCE_DIR}/EcScript)
set(CMAKE_USER_FILE_TIME ${CMAKE_SOURCE_DIR}/file_time_update)

add_custom_command(TARGET ${PROJECT_NAME}.elf PRE_BUILD
        COMMAND ${CMAKE_USER_EC} -input_file_path ${EC_FILE} -output_file_path ${EC_FILE}
        COMMAND ${CMAKE_USER_MAKEFILE} -input_file_path ${MAKE_FILE} -output_file_path ${MAKE_FILE} -name ${PROJECT_NAME}
        COMMAND ${CMAKE_USER_FILE_TIME} -input_file_path ${SYS_CFG_FILE}
        COMMENT "Updating ${MAKE_FILE}")

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_USER} -o ${INFO_BIN_FILE} -k ${BIN_FILE} -n bgm_afc_code
        COMMENT "Building ${HEX_FILE} 
Building ${BIN_FILE}
Building ${INFO_BIN_FILE}")
