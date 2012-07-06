
#ifndef __LOG_H__
#define __LOG_H__ 1

#include <sys/types.h>

#include <stdio.h>
#include <time.h>
#ifdef HAVE_LIBZ
# include <zlib.h>
#endif

#include "msgpack-autosplit.h"

typedef struct LogfileOps_ {
    const char *(*logfile_extension)(void);
    const char *(*logfile_current_file_name)(void);
    int         (*logfile_open)(AppContext * const context,
                                const char * const file_name);
    int         (*logfile_close)(AppContext * const context);
    off_t       (*logfile_ftello)(AppContext * const context);
    ssize_t     (*logfile_write)(AppContext * const context,
                                 const void * const data, const size_t size);
} LogfileOps;

int log_init(AppContext * const context);
int log_set_compression(AppContext * const context, const char * const name);
int log_close(AppContext * const context);
int log_rotate(AppContext * const context);
int log_rotate_if_needed(AppContext * const context);
time_t log_get_delay_before_next(AppContext * const context);
ssize_t log_write(AppContext * const context, const void * const data,
                  const size_t size);
#endif
