
#ifndef __LOG_H__
#define __LOG_H__ 1

#include <time.h>

#include "msgpack-autosplit.h"

int log_close(AppContext * const context);
int log_rotate(AppContext * const context);
int log_rotate_if_needed(AppContext * const context);
time_t log_get_delay_before_next(AppContext * const context);

#endif
