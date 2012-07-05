
#ifndef __LOG_H__
#define __LOG_H__ 1

#include <sys/types.h>

#include <stdio.h>
#include <time.h>
#ifdef HAVE_LIBZ
# include <zlib.h>
#endif

#include "msgpack-autosplit.h"

int log_close(AppContext * const context);
int log_rotate(AppContext * const context);
int log_rotate_if_needed(AppContext * const context);
time_t log_get_delay_before_next(AppContext * const context);
ssize_t log_write(AppContext * const context, const void * const data,
                  const size_t size);
#endif
