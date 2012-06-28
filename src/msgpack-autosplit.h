
#ifndef __MSGPACK_AUTOSPLIT_H__
#define __MSGPACK_AUTOSPLIT_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct AppContext_ {
    const char  *log_dir;
    FILE        *logfile_fp;
    size_t       logfile_soft_limit;
    time_t       logfile_last_rotation;
    time_t       logfile_rotate_after;
    unsigned int logfile_seq; 
} AppContext;

#endif
