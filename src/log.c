
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

static const char *
logfile_extension_none(void)
{
    return "";
}

static const char *
logfile_extension_gzip(void)
{
    return ".gz";
}

static const char *
logfile_current_file_name_none(void)
{
    return LOG_LOGFILE_NAME_CURRENT_BASE;
}

static const char *
logfile_current_file_name_gzip(void)
{
    return LOG_LOGFILE_NAME_CURRENT_BASE ".gz";
}

static int
logfile_open_none(AppContext * const context, const char * const file_name)
{
    assert(context->logfile_enabled == 0);
    context->logfile_fd.fp = fopen(file_name, "a");
    if (context->logfile_fd.fp == NULL) {
        return -1;
    }
    context->logfile_enabled = 1;

    return 0;
}

static int
logfile_open_gzip(AppContext * const context, const char * const file_name)
{
    assert(context->logfile_enabled == 0);
    context->logfile_fd.gzfp = gzopen(file_name, "a");
    if (context->logfile_fd.gzfp == NULL) {
        return -1;
    }
    context->logfile_enabled = 1;

    return 0;
}

static int
logfile_close_none(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.fp != NULL);
    if (fclose(context->logfile_fd.fp) != 0) {
        return -1;
    }
    context->logfile_fd.fp = NULL;

    return 0;
}

static int
logfile_close_gzip(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.gzfp != NULL);
    if (gzclose(context->logfile_fd.gzfp) != 0) {
        return -1;
    }
    context->logfile_fd.gzfp = NULL;

    return 0;
}

static off_t
logfile_ftello_none(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.fp != NULL);
    return ftello(context->logfile_fd.fp);
}

static off_t
logfile_ftello_gzip(AppContext * const context)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.gzfp != NULL);
    return (off_t) gztell(context->logfile_fd.gzfp);
}

static ssize_t
logfile_write_none(AppContext * const context, const void * const data,
                   const size_t size)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.fp != NULL);
    return (ssize_t) fwrite(data, (size_t) 1U, size, context->logfile_fd.fp);
}

static ssize_t
logfile_write_gzip(AppContext * const context, const void * const data,
                   const size_t size)
{
    assert(context->logfile_enabled != 0);
    assert(context->logfile_fd.gzfp != NULL);
    if (size > UINT_MAX) {
        errno = EINVAL;
        return (ssize_t) -1;
    }
    return (ssize_t) gzwrite(context->logfile_fd.gzfp, data,
                             (unsigned int) size);
}

static LogfileOps logfile_ops_none = {
    .logfile_extension = logfile_extension_none,
    .logfile_current_file_name = logfile_current_file_name_none,
    .logfile_open      = logfile_open_none,
    .logfile_close     = logfile_close_none,
    .logfile_ftello    = logfile_ftello_none,
    .logfile_write     = logfile_write_none
};

static LogfileOps logfile_ops_gzip = {
    .logfile_extension = logfile_extension_gzip,
    .logfile_current_file_name = logfile_current_file_name_gzip,
    .logfile_open      = logfile_open_gzip,
    .logfile_close     = logfile_close_gzip,
    .logfile_ftello    = logfile_ftello_gzip,
    .logfile_write     = logfile_write_gzip
};

int
log_set_compression(AppContext * const context, const char * const name)
{
    if (strcasecmp(name, "gzip") == 0) {
        context->log_compression = LOG_COMPRESSION_GZIP;
        context->logfile_ops = &logfile_ops_gzip;
        return 0;
    } else if (strcasecmp(name, "none") == 0) {
        assert(context->log_compression == LOG_COMPRESSION_NONE);
        assert(context->logfile_ops == &logfile_ops_none);
        return 0;
    }
    return -1;
}

static int
log_get_timestamped_logfile_name(AppContext * const context,
                                 char * const timestamped_logfile_name,
                                 const size_t timestamped_logfile_size)
{
    const char   *format_extension;
    const time_t  now = time(NULL);

    format_extension = context->logfile_ops->logfile_extension();
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
    return context->logfile_ops->logfile_write(context, data, size);
}

int
log_close(AppContext * const context)
{
    if (context->logfile_enabled != 0) {
        if (context->logfile_ops->logfile_close(context) != 0) {
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
    current_file_name = context->logfile_ops->logfile_current_file_name();
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
    if (context->logfile_ops
        ->logfile_open(context, current_file_name) != 0) {
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
        context->logfile_ops->logfile_ftello(context) >=
        (off_t) context->logfile_soft_limit) {
        return log_rotate(context);
    }
    return 0;
}

int
log_init(AppContext * const context)
{
    context->logfile_ops = &logfile_ops_none;
    context->log_compression = LOG_COMPRESSION_NONE;
    context->logfile_last_rotation = (time_t) -1;

    return 0;
}
