#ifndef IO_PLATFORM_LIB_H
#define IO_PLATFORM_LIB_H

#include <stdint.h>

#if defined(IO_PLATFORM_POSIX)
typedef const char* dev_open_addr_t;
typedef int dev_con_t;
#elif defined(IO_PLATFORM_STM32)
struct __UART_HandleTypeDef;
typedef struct __UART_HandleTypeDef UART_HandleTypeDef;
typedef UART_HandleTypeDef* dev_open_addr_t;
typedef UART_HandleTypeDef* dev_con_t;
#else
#error "Unsupported platform. Define IO_PLATFORM_POSIX or IO_PLATFORM_STM32"
#endif

int platform_sleep_ms(uint32_t ms);

#endif