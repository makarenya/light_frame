cmake_minimum_required(VERSION 3.10)


project('light_frame')

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/libopencm3/lib/stm32/f1/stm32f103x8.ld)
SET(COMMON_FLAGS "-mthumb -mcpu=cortex-m3 -ggdb -O0 -fno-builtin -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS}  -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3 -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections --specs=nano.specs --specs=nosys.specs -mthumb -mcpu=cortex-m3 -T ${LINKER_SCRIPT}")

add_library(libopencm3
    libopencm3/lib/cm3/vector.c
    libopencm3/lib/cm3/systick.c
    libopencm3/lib/cm3/scb.c
    libopencm3/lib/cm3/nvic.c
    libopencm3/lib/cm3/assert.c
    libopencm3/lib/cm3/sync.c
    libopencm3/lib/cm3/dwt.c
    libopencm3/lib/stm32/f1/adc.c
    libopencm3/lib/stm32/common/adc_common_v1.c
    libopencm3/lib/stm32/can.c
    libopencm3/lib/stm32/desig.c
    libopencm3/lib/stm32/f1/flash.c
    libopencm3/lib/stm32/f1/gpio.c
    libopencm3/lib/stm32/f1/rcc.c
    libopencm3/lib/stm32/f1/rtc.c
    libopencm3/lib/stm32/f1/timer.c
    libopencm3/lib/ethernet/mac.c
    libopencm3/lib/ethernet/mac_stm32fxx7.c
    libopencm3/lib/ethernet/phy.c
    libopencm3/lib/ethernet/phy_ksz80x1.c
    libopencm3/lib/stm32/common/crc_common_all.c
    libopencm3/lib/stm32/common/dac_common_all.c
    libopencm3/lib/stm32/common/dma_common_l1f013.c
    libopencm3/lib/stm32/common/gpio_common_all.c
    libopencm3/lib/stm32/common/i2c_common_v1.c
    libopencm3/lib/stm32/common/iwdg_common_all.c
    libopencm3/lib/stm32/common/pwr_common_v1.c
    libopencm3/lib/stm32/common/timer_common_all.c
    libopencm3/lib/stm32/common/usart_common_all.c
    libopencm3/lib/stm32/common/usart_common_f124.c
    libopencm3/lib/stm32/common/rcc_common_all.c
    libopencm3/lib/stm32/common/exti_common_all.c
    libopencm3/lib/stm32/common/flash_common_all.c
    libopencm3/lib/stm32/common/flash_common_f.c
    libopencm3/lib/stm32/common/flash_common_f01.c
    libopencm3/lib/stm32/common/spi_common_all.c
    libopencm3/lib/stm32/common/spi_common_v1.c
    libopencm3/lib/usb/usb.c
    libopencm3/lib/usb/usb_control.c
    libopencm3/lib/usb/usb_standard.c
    libopencm3/lib/usb/usb_msc.c
    libopencm3/lib/usb/usb_dwc_common.c
    libopencm3/lib/usb/usb_f107.c
    libopencm3/lib/stm32/common/st_usbfs_core.c
    libopencm3/lib/stm32/st_usbfs_v1.c
)

target_include_directories(libopencm3 PUBLIC libopencm3/include)
target_link_directories(libopencm3 PUBLIC libopencm3/lib)
target_compile_definitions(libopencm3 PUBLIC STM32F1)

add_executable(light_frame
    src/main.cpp
    src/string_builder.h
    src/string_builder.cpp
    src/timer_service.h
    src/timer_service.cpp
    src/stream.h
    src/stream.cpp
    src/commands_receiver.h
    src/commands_receiver.cpp
    src/light_selector.h
    src/light_selector.cpp
    src/modes/mode_control.h
    src/modes/mode_control.cpp
    src/modes/light_frame_mode.h
    src/modes/light_frame_mode.cpp
    src/modes/magnet_calibration_mode.h
    src/modes/magnet_calibration_mode.cpp
    src/modes/simple_frame_mode.h
    src/modes/simple_frame_mode.cpp
    src/modes/sleep_mode.h
    src/modes/sleep_mode.cpp

    src/platform/hardware.h
    src/platform/platform_control.h
    src/platform/platform_control.cpp
    src/platform/buttons_control.h
    src/platform/buttons_control.cpp
    src/platform/communication_control.h
    src/platform/communication_control.cpp

    src/platform/adc_control.h
    src/platform/adc_control.cpp
    src/platform/dream_time.h
    src/platform/dream_time.cpp

    src/platform/led_control.h
    src/platform/led_control.cpp
    src/platform/magnet_control.h
    src/platform/magnet_control.cpp
)

include_directories(src)
target_link_libraries(light_frame libopencm3)
