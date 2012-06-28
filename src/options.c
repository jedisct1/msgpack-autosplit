
#include <config.h>

#include <err.h>
#include <getopt.h>
#include <stdlib.h>

#include "gettext.h"
#include "options.h"

static struct option getopt_long_options[] = {
    { "dir", 1, NULL, 'd' },
    { "help", 0, NULL, 'h' },
    { "soft-limit", 1, NULL, 's' },
    { "rotate-after", 1, NULL, 't' },
    { "version", 1, NULL, 'V' },
    { NULL, 0, NULL, 0 }
};

static const char *getopt_options = "d:hs:V:";

static void
options_version(void)
{
    puts(PACKAGE_STRING);
}

static void
options_usage(void)
{
    const struct option *options = getopt_long_options;

    options_version();
    printf("\n%s\n", _("Options:"));
    do {
        printf("  -%c\t--%s%s\n", options->val, options->name,
               options->has_arg ? "=..." : "");
        options++;
    } while (options->name != NULL);
}

static int
options_init_with_default(AppContext * const context)
{
    context->log_dir = NULL;
    context->logfile_soft_limit = (size_t) 100U;
    context->logfile_seq = 0U;
    
    return 0;
}

static int
options_apply(AppContext * const context)
{
    if (context->log_dir == NULL) {
        errx(1, _("Directory not specified"));
    }
    return 0;
}

int
options_parse(AppContext * const context, int argc, char *argv[])
{
    int opt_flag;
    int option_index = 0;

    options_init_with_default(context);
    while ((opt_flag = getopt_long(argc, argv,
                                   getopt_options, getopt_long_options,
                                   &option_index)) != -1) {
        switch (opt_flag) {
        case 'd':
            context->log_dir = optarg;
            break;
        case 'h':
            options_usage();
            exit(0);
        case 't':
            context->logfile_rotate_after = (time_t) strtoul(optarg, NULL, 10);
            break;
        case 's':
            context->logfile_soft_limit = (size_t) strtoul(optarg, NULL, 10);
            break;
        case 'V':
            options_version();
            exit(0);
        default:
            options_usage();
            exit(0);
        }
    }
    options_apply(context);

    return 0;
}
