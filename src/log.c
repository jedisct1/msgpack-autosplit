
#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include "gettext.h"
#include "log.h"

#define LOG_LOGFILE_NAME_CURRENT_BASE ".current"
#define LOG_TIMESTAMPED_LOGFILE_MAX_LENGTH 100U

static int
logfile_open(AppContext * const context, const char * const file_name)
{
    assert(context->logfile_enabled == 0);
    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        context->logfile_fd.fp = fopen(file_name, "a");
        if (context->logfile_fd.fp == NULL) {
            return -1;
        }
        break;
    case LOG_COMPRESSION_GZIP:
        context->logfile_fd.gzfp = gzopen(file_name, "a");
        if (context->logfile_fd.gzfp == NULL) {
            return -1;
        }
        break;
    }
    context->logfile_enabled = 1;

    return 0;
}

static int
logfile_close(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        assert(context->logfile_fd.fp != NULL);
        if (fclose(context->logfile_fd.fp) != 0) {
            return -1;
        }
        context->logfile_fd.fp = NULL;
        break;
    case LOG_COMPRESSION_GZIP:
        assert(context->logfile_fd.gzfp != NULL);
        if (gzclose(context->logfile_fd.gzfp) != 0) {
            return -1;
        }
        context->logfile_fd.gzfp = NULL;
        break;
    }
    return 0;
}

static off_t
logfile_ftello(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        assert(context->logfile_fd.fp != NULL);
        return ftello(context->logfile_fd.fp);

    case LOG_COMPRESSION_GZIP:
        assert(context->logfile_fd.gzfp != NULL);
        return (off_t) gztell(context->logfile_fd.gzfp);
    }
    errno = EINVAL;
    return (ssize_t) -1;
}

int
log_set_compression(AppContext * const context, const char * const name)
{
    if (strcasecmp(name, "gzip") == 0) {
        context->log_compression = LOG_COMPRESSION_GZIP;
        return 0;
    }
    return -1;
}

static const char *
log_get_current_logfile_name(AppContext * const context)
{
    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        return LOG_LOGFILE_NAME_CURRENT_BASE;
    case LOG_COMPRESSION_GZIP:
        return LOG_LOGFILE_NAME_CURRENT_BASE ".gz";
    }
    errno = EINVAL;

    return NULL;
}

static int
log_get_timestamped_logfile_name(AppContext * const context,
                                 char * const timestamped_logfile_name,
                                 const size_t timestamped_logfile_size)
{
    const char   *format_extension;
    const time_t  now = time(NULL);

    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        format_extension = "";
        break;
    case LOG_COMPRESSION_GZIP:
        format_extension = ".gz";
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    if (context->logfile_seq == 0) {
        if ((size_t) snprintf(timestamped_logfile_name,
                              timestamped_logfile_size,
                              "%llu.msgpack%s", (unsigned long long) now,
                              format_extension) > timestamped_logfile_size) {
            return -1;
        }
    }
    if ((size_t) snprintf(timestamped_logfile_name,
                          timestamped_logfile_size,
                          "%llu.%05u.msgpack%s", (unsigned long long) now,
                          context->logfile_seq, format_extension) >
        timestamped_logfile_size) {
        return -1;
    }
    return 0;
}

ssize_t
log_write(AppContext * const context, const void * const data,
          const size_t size)
{
    assert(context->logfile_enabled != 0);
    switch (context->log_compression) {
    case LOG_COMPRESSION_NONE:
        assert(context->logfile_fd.fp != NULL);
        return (ssize_t) fwrite(data, (size_t) 1U, size,
                                context->logfile_fd.fp);
    case LOG_COMPRESSION_GZIP:
        assert(context->logfile_fd.gzfp != NULL);
        if (size > UINT_MAX) {
            break;
        }
        return (ssize_t) gzwrite(context->logfile_fd.gzfp, data,
                                 (unsigned int) size);
    }
    errno = EINVAL;
    return (ssize_t) -1;
}

int
log_close(AppContext * const context)
{
    if (context->logfile_enabled != 0) {
        if (logfile_close(context) != 0) {
            return -1;
        }
        context->logfile_enabled = 0;
    }
    return 0;
}

int
log_rotate(AppContext * const context)
{
    char        timestamped_logfile_name[LOG_TIMESTAMPED_LOGFILE_MAX_LENGTH];
    const char *current_file_name;
    struct stat st;

    if (log_close(context) != 0) {
        return -1;
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
    current_file_name = log_get_current_logfile_name(context);
    assert(current_file_name != NULL);
    if (stat(current_file_name, &st) != 0) {
        if (errno != ENOENT) {
            warn(_("Unable to stat [%s]"), current_file_name);
        }
    } else if (st.st_size > (off_t) 0) {
        if (rename(current_file_name, timestamped_logfile_name) != 0 &&
            errno != ENOENT) {
            warn(_("Unable to rename [%s] to [%s]"),
                 current_file_name, timestamped_logfile_name);
        }
    }
    assert(context->logfile_enabled == 0);
    if (logfile_open(context, current_file_name) != 0) {
        err(1, _("Unable to create [%s]"), current_file_name);
    }
    assert(context->logfile_enabled = 1);

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
        logfile_ftello(context) >= (off_t) context->logfile_soft_limit) {
        return log_rotate(context);
    }
    return 0;
}
