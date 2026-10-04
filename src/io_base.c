#include "io.h"

#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

rx_status_t io_stateful_rx(dev_con_t con, uint16_t guard_time_ms, writer_t* w, i_consumer_t consumer, void* ctx) {
    rx_status_t ret = READ_OK;

    if (w->chunk == 0) {
        ret = READ_INVALID_PARAM;
        goto finish;
    }

    int n_read = 0;
    size_t n_consumed;
    consumer_status_t consume_st = CONSUMER_OK;
    reader_t r = {
            w->buf,
            w->buf_len,
            0,
            0
    };
    bool guard_flag = false;
    while (true) {
        if (w->pos >= w->buf_len) {
            ret = READ_MAX_BUFFER_SIZE_REACHED;
            goto finish;
        }

        n_read = io_rx(con, w->buf + w->pos, MIN(w->chunk, w->buf_len - w->pos));
        if (n_read < 0) {
#if defined(IO_PLATFORM_POSIX)
            if (errno != EAGAIN) {
                ret = READ_KO;
                goto finish;
            }
#else
            ret = READ_KO;
            goto finish;
#endif
            if (guard_time_ms == 0 || guard_flag) {
                break;
            }
            platform_sleep_ms(guard_time_ms);
            guard_flag = true;
            continue;
        } else if (n_read == 0) {
#if defined(IO_PLATFORM_POSIX)
            ret = READ_CONNECTION_CLOSED;
            goto finish;
#else
            if (guard_time_ms == 0 || guard_flag) {
                break;
            }
            platform_sleep_ms(guard_time_ms);
            guard_flag = true;
            continue;
#endif
        }

        w->pos += n_read;

        for (;r.pos < w->pos;) {
            r.chunk = w->pos - r.pos;
            consume_st = consumer(w->buf + r.pos, r.chunk, &n_consumed, ctx);
            if (consume_st == CONSUMER_KO || n_consumed == 0) {
                ret = READ_KO;
                goto finish;
            }
            if (n_consumed > r.chunk) {
                // TODO: Warning read more than expected reading will be desynchronized
                n_consumed = r.chunk;
            }
            if (consume_st == CONSUMER_UNFINISHED) {
                // Need to continue but run out of read space
                // Consumer has to reread the same part again hoping to receive more bytes on the next iter
                if (n_consumed < r.chunk) {
                    // TODO: Warning unfinished but did not read all
                }
                if (r.pos == 0 && w->pos >= w->buf_len) {
                    // TODO: Log an error
                    ret = CONSUMER_NEEDS_MORE_SPACE;
                    goto finish;
                }
                memmove(w->buf, w->buf + r.pos, n_consumed);
                w->pos = n_consumed;
                r.pos = 0;
                break;
            }
            r.pos += n_consumed;
        }

        if (r.pos >= w->pos) {
            w->pos = 0;
            r.pos = 0;
        }
        guard_flag = false;
    }

    if (r.pos < w->pos) {
        // TODO: Warning partial read
        if (r.pos > 0) {
            memmove(w->buf, w->buf + r.pos, w->pos - r.pos);
            w->pos = w->pos - r.pos;
            r.pos = 0;
        }
    }

    finish:
    return ret;
}

rx_status_t io_stateless_rx(dev_con_t con, uint16_t guard_time_ms, uint8_t* b_out, size_t b_len, i_consumer_t consumer, void* ctx) {
    return io_stateful_rx(con,
                          guard_time_ms,
                          &(writer_t) {b_out, b_len, b_len, 0},
                          consumer,
                          ctx);
}
