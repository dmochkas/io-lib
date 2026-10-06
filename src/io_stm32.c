#include "io.h"

#include <string.h>

#include "stm32_hal_al.h"

static int hal_receive(UART_HandleTypeDef* uart, uint8_t* bytes, uint16_t n, uint32_t timeout) {
    if (uart == NULL || bytes == NULL || n == 0) {
        return -1;
    }

    HAL_StatusTypeDef status = HAL_UART_Receive(uart, bytes, n, timeout);
    if (status == HAL_OK) {
        return n;
    }
    if (status == HAL_TIMEOUT) {
        return 0;
    }
    return -1;
}

int platform_sleep_ms(uint32_t ms) {
    HAL_Delay(ms);
    return 0;
}

dev_con_t io_open_serial(dev_open_addr_t port, int baudrate) {
    (void) baudrate;
    return port;
}

int io_rx(dev_con_t con, uint8_t* bytes, int n) {
    if (n <= 0) {
        return n == 0 ? 0 : -1;
    }
    if (con == NULL || bytes == NULL) {
        return -1;
    }

    int received = 0;
    while (received < n) {
        int result = hal_receive(con, bytes + received, 1, 0);
        if (result < 0) {
            return -1;
        }
        if (result == 0) {
            break;
        }
        received += result;
    }
    return received;
}

int io_rx_blocking(dev_con_t con, uint8_t* bytes, int n, int timeout_ms) {
    if (con == NULL || bytes == NULL || n <= 0 || timeout_ms < 0) {
        return -1;
    }

    uint32_t start = HAL_GetTick();
    int received = 0;
    while (received < n) {
        uint32_t elapsed = HAL_GetTick() - start;
        if (elapsed >= (uint32_t) timeout_ms) {
            break;
        }
        int result = hal_receive(con, bytes + received, 1,
                                 (uint32_t) timeout_ms - elapsed);
        if (result < 0) {
            return -1;
        }
        if (result == 0) {
            break;
        }
        received += result;
    }
    return received;
}

int io_rx_line(dev_con_t con, uint8_t* bytes, uint32_t max_len, char* eol_chars) {
    if (con == NULL || bytes == NULL || max_len == 0 || eol_chars == NULL) {
        return -1;
    }

    size_t eol_len = strlen(eol_chars);
    uint32_t total = 0;
    while (total < max_len) {
        uint8_t ch;
        int result = hal_receive(con, &ch, 1, 0);
        if (result < 0) {
            return -1;
        }
        if (result == 0) {
            return (int) total;
        }
        if (memchr(eol_chars, ch, eol_len) != NULL) {
            return (int) total;
        }
        bytes[total++] = ch;
    }
    return -1;
}

int io_tx(dev_con_t con, const uint8_t* bytes, int n) {
    if (n < 0 || n > UINT16_MAX || (n > 0 && (con == NULL || bytes == NULL))) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(con, (uint8_t*) bytes,
                                                  (uint16_t) n, 0);
    return status == HAL_OK ? n : (status == HAL_TIMEOUT ? 0 : -1);
}

int io_tx_blocking(dev_con_t con, const uint8_t* bytes, int n) {
    if (n < 0 || n > UINT16_MAX || (n > 0 && (con == NULL || bytes == NULL))) {
        return -1;
    }
    if (n == 0) {
        return 0;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(con, (uint8_t*) bytes,
                                                  (uint16_t) n, HAL_MAX_DELAY);
    return status == HAL_OK ? n : -1;
}

int io_tx_drain(dev_con_t con) {
    return con == NULL ? -1 : 0;
}

int io_rx_drain(dev_con_t con) {
    if (con == NULL) {
        return -1;
    }

    uint8_t drain_buf[32];
    int result;
    do {
        result = io_rx(con, drain_buf, sizeof(drain_buf));
        if (result < 0) {
            return -1;
        }
    } while (result > 0);
    return 0;
}

int io_rx_drain_n_bytes(dev_con_t con, uint16_t n) {
    if (con == NULL) {
        return -1;
    }

    uint8_t drain_buf[32];
    int drained = 0;
    while (n > 0) {
        uint16_t chunk = n < sizeof(drain_buf) ? n : (uint16_t) sizeof(drain_buf);
        int result = io_rx(con, drain_buf, chunk);
        if (result < 0) {
            return -1;
        }
        if (result == 0) {
            break;
        }
        drained += result;
        n -= (uint16_t) result;
    }
    return drained;
}

void io_close(dev_con_t con) {
    (void) con;
}