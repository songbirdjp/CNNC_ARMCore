
```
ArmCoreV1_QSPI
├─ .cproject
├─ .gitignore
├─ .mxproject
├─ .project
├─ 7312.xml
├─ application
│  ├─ config.h
│  ├─ ethercat
│  │  ├─ ethercat.c
│  │  └─ ethercat.h
│  ├─ fpga
│  │  ├─ fpga_port.c
│  │  ├─ fpga_port.h
│  │  ├─ fpga_rw.c
│  │  └─ fpga_rw.h
│  ├─ freertos.c
│  ├─ FreeRTOSConfig.h
│  ├─ main.c
│  ├─ main.h
│  ├─ nonRealtimeDataProcess.c
│  ├─ nonRealtimeDataProcess.h
│  ├─ syscall.h
│  ├─ syscalls.c
│  ├─ sysmem.c
│  ├─ sys_cfg.c
│  ├─ sys_cfg.h
│  └─ tcp
│     ├─ httpserver.c
│     ├─ httpserver.h
│     ├─ tcp_client.c
│     └─ tcp_client.h
├─ ArmCoreV1.pdf
├─ ArmCoreV1_QSPI 固件状态说明.md
├─ board
│  ├─ board_config
│  │  ├─ inc
│  │  │  ├─ stm32h7xx_hal_conf.h
│  │  │  └─ stm32h7xx_it.h
│  │  └─ src
│  │     ├─ stm32h7xx_hal_msp.c
│  │     ├─ stm32h7xx_it.c
│  │     └─ system_stm32h7xx.c
│  ├─ linker_scripts
│  │  ├─ STM32H723ZGTX_FLASH.ld
│  │  └─ STM32H723ZGTX_RAM.ld
│  └─ startup
│     └─ startup_stm32h723zgtx.s
├─ CMakeLists.txt
├─ CMakeLists_template.txt
├─ components
│  ├─ cm_backtrace
│  │  ├─ cmb_cfg.h
│  │  ├─ cmb_def.h
│  │  ├─ cm_backtrace.c
│  │  ├─ cm_backtrace.h
│  │  ├─ fault_handler
│  │  │  └─ gcc
│  │  │     └─ cmb_fault.S
│  │  └─ Languages
│  │     ├─ en-US
│  │     │  └─ cmb_en_US.h
│  │     ├─ README.md
│  │     └─ zh-CN
│  │        ├─ cmb_zh_CN.h
│  │        └─ cmb_zh_CN_UTF8.h
│  ├─ common
│  │  └─ init_call.h
│  ├─ finsh
│  │  ├─ cmd.c
│  │  ├─ finsh.h
│  │  ├─ finsh_config.h
│  │  ├─ msh.c
│  │  ├─ msh.h
│  │  ├─ msh_file.c
│  │  ├─ msh_parse.c
│  │  ├─ msh_parse.h
│  │  ├─ shell.c
│  │  └─ shell.h
│  ├─ ulog
│  │  ├─ ulog.c
│  │  └─ ulog.h
│  └─ utilities
│     ├─ utilities.c
│     └─ utilities.h
├─ config.cmake
├─ config.h.in
├─ drivers
│  ├─ external_drivers
│  │  ├─ backup_sram
│  │  │  ├─ backup_sram.c
│  │  │  └─ backup_sram.h
│  │  ├─ fram
│  │  │  ├─ fram_port.c
│  │  │  └─ fram_port.h
│  │  ├─ lan9252
│  │  │  ├─ 9252_HW.c
│  │  │  ├─ 9252_HW.h
│  │  │  ├─ applInterface.h
│  │  │  ├─ coeappl.c
│  │  │  ├─ coeappl.h
│  │  │  ├─ ecatappl.c
│  │  │  ├─ ecatappl.h
│  │  │  ├─ ecatcoe.c
│  │  │  ├─ ecatcoe.h
│  │  │  ├─ ecatslv.c
│  │  │  ├─ ecatslv.h
│  │  │  ├─ ecat_def.h
│  │  │  ├─ esc.h
│  │  │  ├─ lan9252_app.c
│  │  │  ├─ lan9252_app.h
│  │  │  ├─ lan9252_appObjects.h
│  │  │  ├─ lan9252_port.c
│  │  │  ├─ lan9252_port.h
│  │  │  ├─ mailbox.c
│  │  │  ├─ mailbox.h
│  │  │  ├─ objdef.c
│  │  │  ├─ objdef.h
│  │  │  ├─ sdoserv.c
│  │  │  ├─ sdoserv.h
│  │  │  ├─ SPIDriver.c
│  │  │  └─ SPIDriver.h
│  │  ├─ sdram
│  │  │  ├─ sdram_fmc_drv.c
│  │  │  └─ sdram_fmc_drv.h
│  │  └─ w5500
│  │     ├─ socket.c
│  │     ├─ socket.h
│  │     ├─ w5500.c
│  │     ├─ w5500.h
│  │     ├─ w5500_port.c
│  │     ├─ w5500_port.h
│  │     ├─ wizchip_conf.c
│  │     └─ wizchip_conf.h
│  └─ internal_drivers
│     ├─ bdma
│     │  ├─ bdma.c
│     │  └─ bdma.h
│     ├─ crc
│     │  ├─ crc.c
│     │  └─ crc.h
│     ├─ dma
│     │  ├─ dma.c
│     │  └─ dma.h
│     ├─ fmc
│     │  ├─ fmc.c
│     │  └─ fmc.h
│     ├─ gpio
│     │  ├─ drv_gpio.c
│     │  ├─ drv_gpio.h
│     │  ├─ gpio.c
│     │  ├─ gpio.h
│     │  ├─ gpio_port.c
│     │  └─ gpio_port.h
│     ├─ iwdg
│     │  ├─ iwdg.c
│     │  └─ iwdg.h
│     ├─ mdma
│     │  ├─ mdma.c
│     │  └─ mdma.h
│     ├─ octospi
│     │  ├─ drv_octospi.c
│     │  ├─ drv_octospi.h
│     │  ├─ octospi.c
│     │  └─ octospi.h
│     ├─ rtc
│     │  ├─ rtc.c
│     │  └─ rtc.h
│     ├─ spi
│     │  ├─ drv_spi.c
│     │  ├─ drv_spi.h
│     │  ├─ spi.c
│     │  └─ spi.h
│     ├─ timer
│     │  ├─ tim.c
│     │  └─ tim.h
│     ├─ uart
│     │  ├─ console.c
│     │  ├─ console.h
│     │  ├─ drv_uart.c
│     │  ├─ drv_uart.h
│     │  ├─ usart.c
│     │  └─ usart.h
│     └─ 放置cubemx生成的片上外设驱动文件，并可添加自己写的驱动.txt
├─ ETHERCAT_CNNCPM.ioc
├─ libraries
│  ├─ CMSIS
│  │  ├─ Device
│  │  │  └─ ST
│  │  │     └─ STM32H7xx
│  │  │        ├─ Include
│  │  │        │  ├─ stm32h723xx.h
│  │  │        │  ├─ stm32h7xx.h
│  │  │        │  └─ system_stm32h7xx.h
│  │  │        ├─ LICENSE.txt
│  │  │        └─ Source
│  │  │           └─ Templates
│  │  ├─ Include
│  │  │  ├─ cmsis_armcc.h
│  │  │  ├─ cmsis_armclang.h
│  │  │  ├─ cmsis_armclang_ltm.h
│  │  │  ├─ cmsis_compiler.h
│  │  │  ├─ cmsis_gcc.h
│  │  │  ├─ cmsis_iccarm.h
│  │  │  ├─ cmsis_version.h
│  │  │  ├─ core_armv81mml.h
│  │  │  ├─ core_armv8mbl.h
│  │  │  ├─ core_armv8mml.h
│  │  │  ├─ core_cm0.h
│  │  │  ├─ core_cm0plus.h
│  │  │  ├─ core_cm1.h
│  │  │  ├─ core_cm23.h
│  │  │  ├─ core_cm3.h
│  │  │  ├─ core_cm33.h
│  │  │  ├─ core_cm35p.h
│  │  │  ├─ core_cm4.h
│  │  │  ├─ core_cm7.h
│  │  │  ├─ core_sc000.h
│  │  │  ├─ core_sc300.h
│  │  │  ├─ mpu_armv7.h
│  │  │  ├─ mpu_armv8.h
│  │  │  └─ tz_context.h
│  │  └─ LICENSE.txt
│  └─ STM32H7xx_HAL_Driver
│     ├─ Inc
│     │  ├─ Legacy
│     │  │  └─ stm32_hal_legacy.h
│     │  ├─ stm32h7xx_hal.h
│     │  ├─ stm32h7xx_hal_cortex.h
│     │  ├─ stm32h7xx_hal_crc.h
│     │  ├─ stm32h7xx_hal_crc_ex.h
│     │  ├─ stm32h7xx_hal_def.h
│     │  ├─ stm32h7xx_hal_dma.h
│     │  ├─ stm32h7xx_hal_dma_ex.h
│     │  ├─ stm32h7xx_hal_exti.h
│     │  ├─ stm32h7xx_hal_flash.h
│     │  ├─ stm32h7xx_hal_flash_ex.h
│     │  ├─ stm32h7xx_hal_gpio.h
│     │  ├─ stm32h7xx_hal_gpio_ex.h
│     │  ├─ stm32h7xx_hal_hsem.h
│     │  ├─ stm32h7xx_hal_i2c.h
│     │  ├─ stm32h7xx_hal_i2c_ex.h
│     │  ├─ stm32h7xx_hal_iwdg.h
│     │  ├─ stm32h7xx_hal_mdma.h
│     │  ├─ stm32h7xx_hal_ospi.h
│     │  ├─ stm32h7xx_hal_pwr.h
│     │  ├─ stm32h7xx_hal_pwr_ex.h
│     │  ├─ stm32h7xx_hal_rcc.h
│     │  ├─ stm32h7xx_hal_rcc_ex.h
│     │  ├─ stm32h7xx_hal_rtc.h
│     │  ├─ stm32h7xx_hal_rtc_ex.h
│     │  ├─ stm32h7xx_hal_sdram.h
│     │  ├─ stm32h7xx_hal_spi.h
│     │  ├─ stm32h7xx_hal_spi_ex.h
│     │  ├─ stm32h7xx_hal_tim.h
│     │  ├─ stm32h7xx_hal_tim_ex.h
│     │  ├─ stm32h7xx_hal_uart.h
│     │  ├─ stm32h7xx_hal_uart_ex.h
│     │  ├─ stm32h7xx_ll_bus.h
│     │  ├─ stm32h7xx_ll_cortex.h
│     │  ├─ stm32h7xx_ll_crc.h
│     │  ├─ stm32h7xx_ll_crs.h
│     │  ├─ stm32h7xx_ll_dma.h
│     │  ├─ stm32h7xx_ll_dmamux.h
│     │  ├─ stm32h7xx_ll_exti.h
│     │  ├─ stm32h7xx_ll_fmc.h
│     │  ├─ stm32h7xx_ll_gpio.h
│     │  ├─ stm32h7xx_ll_hsem.h
│     │  ├─ stm32h7xx_ll_iwdg.h
│     │  ├─ stm32h7xx_ll_lpuart.h
│     │  ├─ stm32h7xx_ll_pwr.h
│     │  ├─ stm32h7xx_ll_rcc.h
│     │  ├─ stm32h7xx_ll_rtc.h
│     │  ├─ stm32h7xx_ll_spi.h
│     │  ├─ stm32h7xx_ll_system.h
│     │  ├─ stm32h7xx_ll_tim.h
│     │  ├─ stm32h7xx_ll_usart.h
│     │  └─ stm32h7xx_ll_utils.h
│     ├─ LICENSE.txt
│     └─ Src
│        ├─ stm32h7xx_hal.c
│        ├─ stm32h7xx_hal_cortex.c
│        ├─ stm32h7xx_hal_crc.c
│        ├─ stm32h7xx_hal_crc_ex.c
│        ├─ stm32h7xx_hal_dma.c
│        ├─ stm32h7xx_hal_dma_ex.c
│        ├─ stm32h7xx_hal_exti.c
│        ├─ stm32h7xx_hal_flash.c
│        ├─ stm32h7xx_hal_flash_ex.c
│        ├─ stm32h7xx_hal_gpio.c
│        ├─ stm32h7xx_hal_hsem.c
│        ├─ stm32h7xx_hal_i2c.c
│        ├─ stm32h7xx_hal_i2c_ex.c
│        ├─ stm32h7xx_hal_iwdg.c
│        ├─ stm32h7xx_hal_mdma.c
│        ├─ stm32h7xx_hal_ospi.c
│        ├─ stm32h7xx_hal_pwr.c
│        ├─ stm32h7xx_hal_pwr_ex.c
│        ├─ stm32h7xx_hal_rcc.c
│        ├─ stm32h7xx_hal_rcc_ex.c
│        ├─ stm32h7xx_hal_rtc.c
│        ├─ stm32h7xx_hal_rtc_ex.c
│        ├─ stm32h7xx_hal_sdram.c
│        ├─ stm32h7xx_hal_spi.c
│        ├─ stm32h7xx_hal_spi_ex.c
│        ├─ stm32h7xx_hal_tim.c
│        ├─ stm32h7xx_hal_tim_ex.c
│        ├─ stm32h7xx_hal_uart.c
│        ├─ stm32h7xx_hal_uart_ex.c
│        └─ stm32h7xx_ll_fmc.c
├─ Microchip-Device-SPI.xml
├─ Middlewares
│  └─ Third_Party
│     └─ FreeRTOS
│        └─ Source
│           ├─ CMSIS_RTOS_V2
│           │  ├─ cmsis_os.h
│           │  ├─ cmsis_os2.c
│           │  ├─ cmsis_os2.h
│           │  ├─ freertos_mpool.h
│           │  └─ freertos_os2.h
│           ├─ croutine.c
│           ├─ event_groups.c
│           ├─ include
│           │  ├─ atomic.h
│           │  ├─ croutine.h
│           │  ├─ deprecated_definitions.h
│           │  ├─ event_groups.h
│           │  ├─ FreeRTOS.h
│           │  ├─ list.h
│           │  ├─ message_buffer.h
│           │  ├─ mpu_prototypes.h
│           │  ├─ mpu_wrappers.h
│           │  ├─ portable.h
│           │  ├─ projdefs.h
│           │  ├─ queue.h
│           │  ├─ semphr.h
│           │  ├─ StackMacros.h
│           │  ├─ stack_macros.h
│           │  ├─ stream_buffer.h
│           │  ├─ task.h
│           │  └─ timers.h
│           ├─ LICENSE
│           ├─ list.c
│           ├─ portable
│           │  ├─ GCC
│           │  │  └─ ARM_CM4F
│           │  │     ├─ port.c
│           │  │     └─ portmacro.h
│           │  └─ MemMang
│           │     └─ heap_4.c
│           ├─ queue.c
│           ├─ stream_buffer.c
│           ├─ tasks.c
│           └─ timers.c
├─ st_link.cfg
├─ test_case
└─ 使用说明
   ├─ 工程修改说明.md
   ├─ 工程修改说明.pdf
   ├─ 文件修改说明.png
   └─ 文件架构.svg

```