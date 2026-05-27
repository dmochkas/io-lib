#ifndef IO_LIB_H
#define IO_LIB_H

#include <stdint.h>

#include "io_platform.h"

/**
 * Open a serial connection.
 * @param open_addr Device address.
 * @param baudrate Serial baud rate.
 * @return Connection handle or a negative error.
 */
dev_con_t io_open_serial(dev_open_addr_t open_addr, int baudrate);

/**
 * Read up to n bytes without waiting.
 * @param con Open connection handle.
 * @param bytes Destination buffer.
 * @param n Maximum number of bytes to read.
 * @return Bytes read, 0, or a negative error.
 */
int io_rx(dev_con_t con, uint8_t* bytes, int n);

/**
 * Read up to n bytes with a timeout.
 * @param con Open connection handle.
 * @param bytes Destination buffer.
 * @param n Maximum number of bytes to read.
 * @param timeout_ms Timeout in milliseconds.
 * @return Bytes read, 0 on timeout, or a negative error.
 */
int io_rx_blocking(dev_con_t con, uint8_t* bytes, int n, int timeout_ms);

/**
 * Read a line into bytes.
 * @param con Open connection handle.
 * @param bytes Destination buffer.
 * @param max_len Maximum number of bytes to store.
 * @param eol_chars Null-terminated set of end-of-line characters.
 * @return Bytes stored or a negative error.
 */
int io_rx_line(dev_con_t con, uint8_t* bytes, uint32_t max_len, char* eol_chars);

/**
 * Write up to n bytes without waiting.
 * @param con Open connection handle.
 * @param bytes Source buffer.
 * @param n Number of bytes to write.
 * @return Bytes written or a negative error.
 */
int io_tx(dev_con_t con, const uint8_t* bytes, int n);

/**
 * Write data with a timeout.
 * @param con Open connection handle.
 * @param bytes Source buffer.
 * @param n Number of bytes to write.
 * @return Bytes written, possibly fewer than requested, or a negative error.
 */
int io_tx_blocking(dev_con_t con, const uint8_t* bytes, int n);

/**
 * Flush pending transmitted bytes.
 * @param con Open connection handle.
 * @return 0 on success, or a negative error.
 */
int io_tx_drain(dev_con_t con);

/**
 * Discard unread bytes from the receive buffer.
 * @param con Open connection handle.
 * @return 0 on success, or a negative error.
 */
int io_rx_drain(dev_con_t con);

/**
 * Backend-specific helper to drain up to n received bytes.
 * @param con Open connection handle.
 * @param n Number of bytes to drain.
 * @return Drained bytes or a negative error.
 */
int io_rx_drain_n_bytes(dev_con_t con, uint16_t n);

/**
 * Close a connection and release its resources.
 * @param con Open connection handle.
 */
void io_close(dev_con_t con);


#endif // IO_LIB_H