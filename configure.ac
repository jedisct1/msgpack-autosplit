
AC_PREREQ(2.61)
AC_INIT(msgpack-autosplit, 0.1, http://sourceforge.net)
AC_CONFIG_SRCDIR([src/app.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9 dist-bzip2])
AM_MAINTAINER_MODE
AM_DEP_TRACK

AC_SUBST(VERSION)
ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

LX_CFLAGS=${CFLAGS-NONE}
AC_PROG_CC_C99
AC_USE_SYSTEM_EXTENSIONS

AX_CHECK_COMPILE_FLAG([-fPIC], [
  AX_CHECK_LINK_FLAG([-fPIC],
    [CFLAGS="$CFLAGS -fPIC"]
  )
])
AX_CHECK_COMPILE_FLAG([-fPIE], [
  AX_CHECK_LINK_FLAG([-fPIE],
    [CFLAGS="$CFLAGS -fPIE"]
  )
])

AX_CHECK_COMPILE_FLAG([-fno-strict-aliasing], [CFLAGS="$CFLAGS -fno-strict-aliasing"])
AX_CHECK_COMPILE_FLAG([-fno-strict-overflow], [CFLAGS="$CFLAGS -fno-strict-overflow"])

AS_IF([echo `(uname -s) 2>/dev/null` | $GREP "CYGWIN" > /dev/null], [
  AX_CHECK_LINK_FLAG([-Wl,--dynamicbase], [LDFLAGS="$LDFLAGS -Wl,--dynamicbase"])
  AX_CHECK_LINK_FLAG([-Wl,--nxcompat], [LDFLAGS="$LDFLAGS -Wl,--nxcompat"])
], [
  AS_IF([test `(uname -s) 2>/dev/null` = "DragonFly"],
    [
      AX_CHECK_COMPILE_FLAG([-fstack-protector], [
        AX_CHECK_LINK_FLAG([-fstack-protector],
          [CFLAGS="$CFLAGS -fstack-protector"]
        )
      ])
    ],
    [
      AX_CHECK_COMPILE_FLAG([-fstack-protector-all], [
        AX_CHECK_LINK_FLAG([-fstack-protector-all],
          [CFLAGS="$CFLAGS -fstack-protector-all"]
        )
      ])
    ]
  )
  AS_IF([test "x$ac_compiler_gnu" = "xyes"],[
    AS_IF([$CC --version 2>/dev/null | $GREP "Free Software Foundation" > /dev/null 2>&1],[
      CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"
      AX_CHECK_COMPILE_FLAG([--param ssp-buffer-size=1], [CFLAGS="$CFLAGS --param ssp-buffer-size=1"])
    ])
  ])
])

AX_CHECK_COMPILE_FLAG([-Winit-self], [CFLAGS="$CFLAGS -Winit-self"])
AX_CHECK_COMPILE_FLAG([-Wwrite-strings], [CFLAGS="$CFLAGS -Wwrite-strings"])
AX_CHECK_COMPILE_FLAG([-Wdiv-by-zero], [CFLAGS="$CFLAGS -Wdiv-by-zero"])

AC_ARG_VAR([CWFLAGS], [define to compilation flags for generating extra warnings])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wall], [CWFLAGS="$CWFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wextra], [CWFLAGS="$CWFLAGS -Wextra"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wno-unknown-warning-option], [CWFLAGS="$CWFLAGS -Wno-unknown-warning-option"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wbad-function-cast], [CWFLAGS="$CWFLAGS -Wbad-function-cast"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcast-align], [CWFLAGS="$CWFLAGS -Wcast-align"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcast-qual], [CWFLAGS="$CWFLAGS -Wcast-qual"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wchar-subscripts], [CWFLAGS="$CWFLAGS -Wchar-subscripts"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wcomment], [CWFLAGS="$CWFLAGS -Wcomment"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wfloat-equal], [CWFLAGS="$CWFLAGS -Wfloat-equal"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wformat=2], [CWFLAGS="$CWFLAGS -Wformat=2"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wimplicit], [CWFLAGS="$CWFLAGS -Wimplicit"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-declarations], [CWFLAGS="$CWFLAGS -Wmissing-declarations"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wmissing-prototypes], [CWFLAGS="$CWFLAGS -Wmissing-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wnormalized=id], [CWFLAGS="$CWFLAGS -Wnormalized=id"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Woverride-init], [CWFLAGS="$CWFLAGS -Woverride-init"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wparentheses], [CWFLAGS="$CWFLAGS -Wparentheses"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wpointer-arith], [CWFLAGS="$CWFLAGS -Wpointer-arith"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wredundant-decls], [CWFLAGS="$CWFLAGS -Wredundant-decls"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wstrict-prototypes], [CWFLAGS="$CWFLAGS -Wstrict-prototypes"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wswitch-enum], [CWFLAGS="$CWFLAGS -Wswitch-enum"])
AX_CHECK_COMPILE_FLAG([$CWFLAGS -Wvariable-decl], [CWFLAGS="$CWFLAGS -Wvariable-decl"])


AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS="$LDFLAGS -Wl,-z,relro"])
AX_CHECK_LINK_FLAG([-Wl,-z,now], [LDFLAGS="$LDFLAGS -Wl,-z,now"])
AX_CHECK_LINK_FLAG([-Wl,-z,noexecstack], [LDFLAGS="$LDFLAGS -Wl,-z,noexecstack"])

AC_ARG_VAR([RONN], [path to the ronn text processor])
AC_PATH_PROG([RONN], [ronn], [#])

AS_IF([test -d /usr/local/include], [
  CPPFLAGS="$CPPFLAGS -I/usr/local/include"
])

AS_IF([test -d /usr/local/lib], [
  LDFLAGS="$LDFLAGS -L/usr/local/lib"
])

LT_INIT
AM_GNU_GETTEXT([external])

AC_SYS_LARGEFILE

AC_CHECK_HEADERS([sandbox.h])
AC_CHECK_FUNCS([sandbox_init])

AC_C_RESTRICT

AC_CHECK_HEADER([CoreServices/CoreServices.h],
  [LIBS="$LIBS -framework CoreFoundation -framework CoreServices"])

AC_HEADER_ASSERT

AC_CHECK_LIB([msgpack], [msgpack_version])
AC_CHECK_LIB([z], [gzopen])

AC_ARG_ENABLE(debug,
[AS_HELP_STRING(--enable-debug,For maintainers only - please do not use)],
[
  AS_IF([test "x$LX_CFLAGS" = "xNONE"], [
    nxflags=""
    for flag in `echo $CFLAGS`; do
      case "$flag" in
        -O*) ;;
        -g*) ;;
        *) AS_VAR_APPEND([nxflags], [" $flag"]) ;;
      esac
    done
    CFLAGS="$nxflags -O0 -g3"
  ])
  CPPFLAGS="$CPPFLAGS -DDEBUG=1"
])

AC_ARG_WITH(safecode,
[AS_HELP_STRING(--with-safecode,For maintainers only - please do not use)],
[ AS_IF([test "x$withval" = "xyes"], [
    AC_ARG_VAR([SAFECODE_HOME], [set to the safecode base directory])
    : ${SAFECODE_HOME:=/opt/safecode}
    LDFLAGS="$LDFLAGS -L${SAFECODE_HOME}/lib"
    LIBS="$LIBS -lsc_dbg_rt -lpoolalloc_bitmap -lstdc++"
    CFLAGS="$CFLAGS -fmemsafety"
  ])
])

AC_SUBST([MAINT])

AC_CONFIG_FILES([Makefile man/Makefile po/Makefile.in src/Makefile test/Makefile])
AC_OUTPUT
