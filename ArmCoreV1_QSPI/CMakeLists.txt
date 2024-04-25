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
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
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
message("git branch: ${GIT_BRANCH}")

set(GIT_HASH "")
get_git_hash(GIT_HASH)
message("git hash: ${GIT_HASH}")

configure_file(config.h.in "../application/user_app/common/config.h")

include_directories(
    libraries/CMSIS/Device/ST/STM32H7xx/Include 
    libraries/CMSIS/Include 
    libraries/STM32H7xx_HAL_Driver/Inc 
    libraries/STM32H7xx_HAL_Driver/Inc/Legacy 
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 
    Middlewares/Third_Party/FreeRTOS/Source/include 
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F 
    drivers/external_drivers/backup_sram 
    drivers/external_drivers/fram 
    drivers/external_drivers/lan9252 
    drivers/external_drivers/sdram 
    drivers/external_drivers/w5500 
    drivers/internal_drivers/bdma 
    drivers/internal_drivers/crc 
    drivers/internal_drivers/dma 
    drivers/internal_drivers/flash  
    drivers/internal_drivers/fmc 
    drivers/internal_drivers/gpio 
    drivers/internal_drivers/iwdg 
    drivers/internal_drivers/mdma 
    drivers/internal_drivers/octospi 
    drivers/internal_drivers/rtc 
    drivers/internal_drivers/spi 
    drivers/internal_drivers/timer 
    drivers/internal_drivers/uart 
    components/cm_backtrace 
    components/common 
    components/console 
    components/gpio 
    components/gpio_imitate 
    components/semaphore 
    components/shell 
    components/ulog 
    components/utilities 
    board/board_config/inc 
    application/ethercat 
    application/fpga 
    application/system_common 
    application/tcp 
    application/user_app/common 
    application/user_app/mlc)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx)

file(GLOB_RECURSE SOURCES 
    "libraries/*.*" 
    "Middlewares/*.*" 
    "drivers/*.*" 
    "components/*.*" 
    "board/*.*" 
    "application/ethercat/*.*" 
    "application/tcp/*.*" 
    "application/fpga/*.*" 
    "application/system_common/*.*" 
    "application/user_app/common/*.*" 
    "application/user_app/mlc/*.*")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/board/linker_scripts/STM32H723ZGTX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m7 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
