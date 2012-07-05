
#ifndef __MSGPACK_AUTOSPLIT_H__
#define __MSGPACK_AUTOSPLIT_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef HAVE_LIBZ
# include <zlib.h>
#endif

typedef enum LogCompression_ {
    LOG_COMPRESSION_NONE, LOG_COMPRESSION_GZIP
} LogCompression;

typedef union LogfileFD_ {
    FILE   *fp;
#ifdef HAVE_LIBZ
    gzFile  gzfp;
#endif
} LogfileFD;

typedef struct AppContext_ {
    const char    *log_dir;
    LogfileFD      logfile_fd;
    size_t         logfile_soft_limit;
    time_t         logfile_last_rotation;
    time_t         logfile_rotate_after;
    unsigned int   logfile_seq;
    LogCompression log_compression;
    _Bool          logfile_enabled;
} AppContext;

#endif
