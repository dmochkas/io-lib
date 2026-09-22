cmake_minimum_required(VERSION 3.10)
project(io-lib VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)

include(config.cmake)

add_library(${IO_LIB})

if (IO_PLATFORM STREQUAL "stm32")
    target_compile_definitions(${IO_LIB} PUBLIC IO_PLATFORM_STM32)
else () # POSIX is the default
    target_compile_definitions(${IO_LIB} PUBLIC IO_PLATFORM_POSIX)
endif ()

add_subdirectory(include)
add_subdirectory(src)

install(TARGETS ${IO_LIB}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES
        include/io.h
        include/io_platform.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

