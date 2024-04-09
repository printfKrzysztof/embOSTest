cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	USE_HAL_DRIVER 
	STM32F303xE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../embOS
    ../../Core/Inc
    ../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy
    ../../Drivers/STM32F3xx_HAL_Driver/Inc
    ../../Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ../../Drivers/CMSIS/Include
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Include
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Include/GCC
)

target_sources(stm32cubemx INTERFACE
    ../../Core/Src/main.c
    ../../Core/Src/stm32f3xx_it.c
    ../../Core/Src/stm32f3xx_hal_msp.c
    ../../Core/Src/stm32f3xx_hal_timebase_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_exti.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart_ex.c
    ../../Core/Src/system_stm32f3xx.c
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/JLINKMEM_Process.c
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/OS_Error.c
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/RTOSInit.c
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Source/GCC/OS_ThreadSafe.c
    ../../Core/Src/sysmem.c
    ../../Core/Src/syscalls.c
    ../../startup_stm32f303xe.s
)

target_link_directories(stm32cubemx INTERFACE
    ../../Middlewares/Third_Party/SEGGER_RTOS_embOS/embOS/Library/GCC
)

target_link_libraries(stm32cubemx INTERFACE 
	:libosT7VHLR.a
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


