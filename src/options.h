
#ifndef __OPTIONS_H__
#define __OPTIONS_H__ 1

#include "msgpack-autosplit.h"

#define LOGFILE_SOFT_LIMIT_DEFAULT 10000000U

int options_parse(AppContext * const context, int argc, char *argv[]);

#endif
