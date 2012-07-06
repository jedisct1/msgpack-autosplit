
#include <config.h>

#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <msgpack.h>
#include <poll.h>
#ifdef HAVE_SANDBOX_H
# include <sandbox.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "gettext.h"
#include "log.h"
#include "msgpack-autosplit.h"
#include "options.h"
#include "safe_rw.h"

#ifndef INPUT_BUFFER_SIZE
# define INPUT_BUFFER_SIZE 100000
#endif

static int
app_init_locale(void)
{
    setlocale(LC_ALL, "");
    (void) bindtextdomain(PACKAGE, LOCALEDIR);
    (void) textdomain(PACKAGE);

    return 0;
}

static int
app_sandbox(void)
{
#ifdef HAVE_SANDBOX_LIMIT
    char *errmsg;
    assert(sandbox_init(kSBXProfileNoNetwork, SANDBOX_NAMED, &errmsg) == 0);
    sandbox_free_error(errmsg);
#endif
    assert(setrlimit(RLIMIT_NPROC,
                     & (struct rlimit) { .rlim_cur = 0, .rlim_max = 0 }) == 0);
    return 0;
}

static int
app_chdir_to_log_dir(const AppContext * const context)
{
    if (mkdir(context->log_dir, (mode_t) 0755) != 0 && errno != EEXIST) {
        err(1, _("Unable to create the directory: [%s]"), context->log_dir);
    }
    if (chdir(context->log_dir) != 0) {
        err(1, _("Unable to open the directory: [%s]"), context->log_dir);
    }
    if (geteuid() == (uid_t) 0U &&
        (chroot(context->log_dir) != 0 || chdir("/") != 0)) {
        err(1, _("Unable to chroot(2) to: [%s]"), context->log_dir);
    }
    return 0;
}

static int
app_poll_stream(AppContext * const context)
{
    struct pollfd    poll_fd = { .fd = STDIN_FILENO, .events = POLLIN };
    time_t           delay_before_next;
    int              poll_timeout;
    int              poll_ret;

    delay_before_next = log_get_delay_before_next(context);
    if (delay_before_next == (time_t) -1 ||
        (time_t) (INT_MAX / 1000) < delay_before_next) {
        poll_timeout = -1;
    } else {
        poll_timeout = (int) delay_before_next * 1000;
    }
    while ((poll_ret = poll(&poll_fd, (nfds_t) 1U, poll_timeout)) == -1 &&
           errno == EINTR);
    assert(poll_ret <= 1);

    return poll_ret;
}

static int
app_process_stream(AppContext * const context)
{
    char             input[INPUT_BUFFER_SIZE];
    msgpack_sbuffer  sbuf;
    msgpack_unpacked pac;
    ssize_t          nbread;
    size_t           offset;
    size_t           poffset = (size_t) 0U;
    _Bool            force_rotate = 0;

    msgpack_sbuffer_init(&sbuf);
    msgpack_unpacked_init(&pac);
    for (;;) {
        switch (app_poll_stream(context)) {
        case -1:
            err(1, "poll");
        case 0:
            log_rotate_if_needed(context);
            continue;
        }
        nbread = safe_read_partial(STDIN_FILENO, input, sizeof input);
        if (nbread == -1) {
            warn(_("Error while reading the input"));
            return -1;
        }
        if (nbread == 0) {
            break;
        }
        if (msgpack_sbuffer_write(&sbuf, input, nbread) != 0) {
            warn(_("Unable to expand a sbuffer"));
            return -1;
        }
        while (sbuf.size > poffset) {
            offset = (size_t) 0U;
            if (msgpack_unpack_next(&pac, sbuf.data + poffset,
                                    sbuf.size - poffset, &offset) == 0) {
                break;
            }
            if (log_write(context, sbuf.data + poffset,
                          offset) != (size_t) 1U) {
                warnx(_("Error when writing a record"));
                force_rotate = 1;
            }
            poffset += offset;
        }
        if (poffset >= sbuf.size) {
            msgpack_sbuffer_clear(&sbuf);
        } else {
            assert(sbuf.size > poffset);
            memmove(sbuf.data, sbuf.data + poffset, sbuf.size - poffset);
            sbuf.size -= poffset;
        }
        poffset = (size_t) 0U;
        if (force_rotate != 0) {
            log_rotate(context);
        } else {
            log_rotate_if_needed(context);
        }
    }
    msgpack_unpacked_destroy(&pac);
    msgpack_sbuffer_destroy(&sbuf);

    return 0;
}

static int
app_context_init(AppContext * const context)
{
    memset(context, 0, sizeof *context);
    context->log_dir = NULL;

    return 0;
}

int
main(int argc, char *argv[])
{
    AppContext context;

    app_init_locale();
    app_sandbox();
    app_context_init(&context);
    log_init(&context);
    options_parse(&context, argc, argv);
    app_chdir_to_log_dir(&context);
    if (log_rotate(&context) != 0) {
        err(1, "log_rotate");
    }
    assert(context.logfile_enabled != 0);
    app_process_stream(&context);
    if (log_rotate(&context) != 0) {
        warn("log_rotate");
    }
    log_close(&context);

    return 0;
}
