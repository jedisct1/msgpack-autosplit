
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
    msgpack_unpacker mpac;
    msgpack_unpacked pac;
    ssize_t          nbread;
    _Bool            force_rotate = 0;

    if (msgpack_unpacker_init(&mpac, MSGPACK_UNPACKER_INIT_BUFFER_SIZE) == 0) {
        errx(1, _("Unable to initialize a MessagePack unpacker"));
    }
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
        msgpack_unpacker_reserve_buffer(&mpac, (size_t) nbread);
        memcpy(msgpack_unpacker_buffer(&mpac), input, (size_t) nbread);
        msgpack_unpacker_buffer_consumed(&mpac, (size_t) nbread);
        if (msgpack_unpacker_next(&mpac, &pac) == 0) {
            continue;
        }
        if (fwrite(mpac.buffer, mpac.off,
                   (size_t) 1U, context->logfile_fp) != (size_t) 1U) {
            warnx(_("Error when writing a record"));
            force_rotate = 1;
        }
        msgpack_zone_free(msgpack_unpacked_release_zone(&pac));
        memmove(mpac.buffer, mpac.buffer + mpac.off, mpac.used - mpac.off);
        mpac.used -= mpac.off;
        mpac.off = 0;
        mpac.free += mpac.off;
        if (force_rotate != 0) {
            log_rotate(context);
        } else {
            log_rotate_if_needed(context);
        }
    }
    msgpack_unpacker_destroy(&mpac);

    return 0;
}

static int
app_context_init(AppContext * const context)
{
    memset(context, 0, sizeof *context);
    context->logfile_fp = NULL;
    context->logfile_last_rotation = (time_t) -1;

    return 0;
}

int
main(int argc, char *argv[])
{
    AppContext context;

    app_init_locale();
    app_sandbox();
    app_context_init(&context);
    options_parse(&context, argc, argv);
    app_chdir_to_log_dir(&context);
    log_rotate(&context);
    assert(context.logfile_fp != NULL);
    app_process_stream(&context);
    log_rotate(&context);

    return 0;
}
