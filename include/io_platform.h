#ifndef IO_PLATFORM_LIB_H
#define IO_PLATFORM_LIB_H

#if defined(IO_PLATFORM_POSIX) | defined(IO_PLATFORM_STM32) | defined(IO_PLATFORM_ESP32)
typedef const char* dev_open_addr_t;
typedef int dev_con_t;
#else
#error "Unsupported platform. Define one of the following PLATFORM_POSIX, PLATFORM_STM32, PLATFORM_ESP32"
#endif

#endif