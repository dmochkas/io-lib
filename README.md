# io-lib

`io-lib` is a small C I/O abstraction library for embedded and embedded-adjacent platforms.

It is intended to provide a consistent interface for basic serial/device communication across:

- POSIX-compliant platforms
- STM32 targets
- ESP32 targets

The first backend scenario is POSIX-style serial I/O, but the API is designed to be portable so platform-specific implementations can share the same application-facing interface.

## Goals

- Hide platform-specific open/read/write/close details behind a common API
- Support blocking and non-blocking transfers
- Provide line-oriented receive helpers for serial console style workflows
- Keep the interface lightweight enough for embedded use

## Core API

The basic I/O interface is centered around a connection handle and a device address type:

- `dev_open_addr_t` — platform-specific open address, such as a device path on POSIX systems
- `dev_con_t` — platform-specific connection handle

Typical API surface:

```c
#pragma once

#include <stdint.h>

#define PLATFORM_POSIX

#ifdef PLATFORM_POSIX
typedef char* dev_open_addr_t;
typedef int dev_con_t;
#endif

dev_con_t io_open_serial(dev_open_addr_t open_addr, int baudrate);

int io_rx(dev_con_t con, uint8_t* bytes, int n);

int io_rx_blocking(dev_con_t con, uint8_t* bytes, int n, int timeout_ms);

int io_rx_line(dev_con_t con, uint8_t* bytes, uint32_t max_len, char* eol_chars);

int io_tx(dev_con_t con, const uint8_t* bytes, int n);

int io_tx_blocking(dev_con_t con, const uint8_t* bytes, int n, int timeout_ms);

int io_tx_drain(dev_con_t con);

int io_tx_drain_n_bytes(dev_con_t con, uint16_t n);

void io_close(dev_con_t con);
```

## Function overview

### Opening and closing

- `io_open_serial(...)` opens a serial connection using a platform-specific address and baud rate.
- `io_close(...)` closes the connection and releases any platform resources.

### Receiving data

- `io_rx(...)` reads up to `n` bytes.
- `io_rx_blocking(...)` waits for data until the requested number of bytes is received or the timeout expires.
- `io_rx_line(...)` reads a line into a buffer and stops at one of the provided end-of-line characters.

### Transmitting data

- `io_tx(...)` writes up to `n` bytes.
- `io_tx_blocking(...)` waits until the data is queued/sent or the timeout expires.
- `io_tx_drain(...)` waits until pending transmitted data has been fully flushed.
- `io_tx_drain_n_bytes(...)` waits until at least `n` bytes have been drained from the transmit path.

## Platform notes

### POSIX

On POSIX systems, the connection is typically represented as a file descriptor and the open address is usually a device path such as:

- `/dev/ttyUSB0`
- `/dev/ttyACM0`

This makes it suitable for Linux, macOS, and other POSIX-like environments.

### STM32 and ESP32

For STM32 and ESP32, the same high-level API can be backed by native UART/serial drivers.

The exact implementation details may differ by board, SDK, or HAL, but the application code should remain largely unchanged.

## Example usage

```c
#include "io.h"

int main(void) {
	dev_con_t con = io_open_serial("/dev/ttyUSB0", 115200);
	if (con < 0) {
		return 1;
	}

	const uint8_t msg[] = "hello\r\n";
	io_tx_blocking(con, msg, (int)(sizeof(msg) - 1), 1000);

	uint8_t buffer[64];
	int received = io_rx_line(con, buffer, sizeof(buffer), "\r\n");
	(void)received;

	io_close(con);
	return 0;
}
```

## Build

This repository uses CMake.

Typical workflow:

```bash
cmake -S . -B build
cmake --build build
```

If you are integrating `io-lib` into another project, add this repository as a subdirectory or include it as a library target in your existing CMake setup.

## Project status

The project currently defines the library direction and API shape for embedded serial I/O portability.

Planned areas for growth typically include:

- concrete POSIX backend implementation
- STM32 UART backend
- ESP32 UART backend
- example applications
- tests and simulation-friendly abstractions

## Contributing

Contributions are welcome, especially for platform backends, examples, and API refinements.

When contributing, try to keep the public API small, portable, and consistent across targets.

