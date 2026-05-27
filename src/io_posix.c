#include "io.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

dev_con_t io_open_serial(dev_open_addr_t port, int baudrate) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        return -1;
    }

    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        close(fd);
        return -1;
    }

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        close(fd);
        return -1;
    }
    tcflush(fd, TCIOFLUSH);

    return fd;
}

int io_rx(dev_con_t con, uint8_t* bytes, int n) {
    return (int) read(con, bytes, (size_t) n);
}

int io_rx_blocking(dev_con_t con, uint8_t* bytes, int n, int timeout_ms) {
    if (bytes == NULL || n <= 0) {
        errno = EINVAL;
        goto error;
    }

    struct pollfd pfd = {
            .fd = con,
            .events = POLLIN,
            .revents = 0,
    };

    int poll_st;
    do {
        poll_st = poll(&pfd, 1, timeout_ms);
        if (poll_st == 0) {
            return 0;
        }
        if (poll_st < 0) {
            goto error;
        }
        if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
            goto error;
        }
        if ((pfd.revents & POLLIN) != 0) {
            break;
        }
    } while (1);

    int received = 0;
    while (received < n) {
        ssize_t read_st = read(con, bytes + received, (size_t) (n - received));
        if (read_st <= 0) {
            if (read_st == 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            goto error;
        }

        received += (int) read_st;
    }

    return received;

    error:
    return -1;
}

int io_rx_line(dev_con_t con, uint8_t* bytes, uint32_t max_len, char* eol_chars) {
    size_t chars_len = strlen(eol_chars);
    int total = 0;
    bool eol_flag = false;
    while (total < max_len) {
        char ch;
        ssize_t n = read(con, &ch, 1);
        if (n < 0) {
            if (errno == EAGAIN && eol_flag) {
                return total;
            }
            break;
        }
        if (n == 0) {
            return total;
        }

        if (memchr(eol_chars, ch, chars_len) != NULL) {
            eol_flag = true;
            continue;
        } else if (eol_flag) {
            return total;
        }
        bytes[total++] = ch;
    }

    return -1;
}

int io_tx(dev_con_t con, const uint8_t* bytes, int n) {
    return (int) write(con, bytes, (size_t) n);
}

int io_tx_blocking(dev_con_t con, const uint8_t* bytes, int n) {
    if (n <= 0) {
        return 0;
    }

    int sent = 0;
    while (sent < n) {
        ssize_t write_st;
        write_st = write(con, bytes + sent, (size_t) (n - sent));
        if (write_st <= 0) {
            if (write_st == 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            goto error;
        }

        sent += (int) write_st;
    }
    if (tcdrain(con) < 0) {
        goto error;
    }

    return sent;

    error:
    return -1;
}

int io_tx_drain(dev_con_t con) {
    return tcdrain(con);
}

int io_rx_drain(dev_con_t con) {
    return tcflush(con, TCIFLUSH);
}

int io_rx_drain_n_bytes(dev_con_t con, uint16_t n) {
    uint8_t drain_buf[n];
    return (int) read(con, drain_buf, n);
}

void io_close(dev_con_t con) {
    close(con);
}