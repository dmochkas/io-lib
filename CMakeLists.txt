cmake_minimum_required(VERSION 3.10)
project(io-lib C)

include(config.cmake)

add_library(${IO_LIB})

if (IO_PLATFORM STREQUAL "stm32")
    target_compile_definitions(${IO_LIB} PUBLIC IO_PLATFORM_STM32)
else () # POSIX is the default
    target_compile_definitions(${IO_LIB} PUBLIC IO_PLATFORM_POSIX)
endif ()

add_subdirectory(include)
add_subdirectory(src)
