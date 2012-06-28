
#include <config.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "gettext.h"
#include "log.h"

#define LOG_LOGFILE_NAME_CURRENT ".current"
#define LOG_TIMESTAMPED_LOGFILE_MAX_LENGTH 50U

static int
log_get_timestamped_logfile_name(AppContext * const context,
                                 char * const timestamped_logfile_name,
                                 const size_t timestamped_logfile_size)
{
    const time_t now = time(NULL);

    if (context->logfile_seq == 0) {
        if ((size_t) snprintf(timestamped_logfile_name,
                              timestamped_logfile_size,
                              "%llu.msgpack", (unsigned long long) now) >
            timestamped_logfile_size) {
            return -1;
        }
    }
    if ((size_t) snprintf(timestamped_logfile_name,
                          timestamped_logfile_size,
                          "%llu.%05u.msgpack", (unsigned long long) now,
                          context->logfile_seq) >
        timestamped_logfile_size) {
        return -1;
    }
    return 0;
}

int
log_rotate(AppContext * const context)
{
    char timestamped_logfile_name[LOG_TIMESTAMPED_LOGFILE_MAX_LENGTH];

    if (context->logfile_fp != NULL) {
        if (fclose(context->logfile_fp) != 0) {
            return -1;
        }
        context->logfile_fp = NULL;
    }
    context->logfile_seq = 0U;
    do {
        if (log_get_timestamped_logfile_name
            (context, timestamped_logfile_name,
                sizeof timestamped_logfile_name) != 0) {
            return -1;
        }
        if (context->logfile_seq >= 0xFFFF) {
            return -1;
        }
        context->logfile_seq++;
    } while (access(timestamped_logfile_name, F_OK) == 0);
    if (rename(LOG_LOGFILE_NAME_CURRENT, timestamped_logfile_name) != 0 &&
        errno != ENOENT) {
        warn(_("Unable to rename [%s] to [%s]"),
             LOG_LOGFILE_NAME_CURRENT, timestamped_logfile_name);
    }
    context->logfile_fp = fopen(LOG_LOGFILE_NAME_CURRENT, "a+");
    if (context->logfile_fp == NULL) {
        errx(1, _("Unable to create [%s]"), LOG_LOGFILE_NAME_CURRENT);
    }
    return 0;
}

time_t
log_get_delay_before_next(AppContext * const context)
{
    time_t elapsed;
    time_t now;    
    
    if (context->logfile_rotate_after == (time_t) -1) {
        return (time_t) -1;
    }
    now = time(NULL);
    if (context->logfile_last_rotation == (time_t) -1 ||
        now < context->logfile_last_rotation) {
        context->logfile_last_rotation = now;
        return context->logfile_last_rotation;
    }
    assert(now >= context->logfile_last_rotation);
    elapsed = now - context->logfile_last_rotation;
    if (elapsed >= context->logfile_rotate_after) {
        return (time_t) 0U;
    }
    return context->logfile_rotate_after - elapsed;
}

int
log_rotate_if_needed(AppContext * const context)
{
    if (log_get_delay_before_next(context) == (time_t) 0 ||
        ftello(context->logfile_fp) >= (off_t) context->logfile_soft_limit) {
        return log_rotate(context);
    }
    return 0;
}
