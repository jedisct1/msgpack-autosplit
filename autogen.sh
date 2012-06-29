#! /bin/sh

if [ -x "`which autoreconf 2>/dev/null`" ] ; then
   exec autoreconf -ivf
fi

gettextize

if glibtoolize --version > /dev/null 2>&1; then
  LIBTOOLIZE='glibtoolize'
else
  LIBTOOLIZE='libtoolize'
fi

$LIBTOOLIZE && \
aclocal -I m4 && \
autoheader && \
automake --add-missing --force-missing --include-deps && \
autoconf -I m4

