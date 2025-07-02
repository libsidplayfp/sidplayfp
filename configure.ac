dnl Process this file with autoconf to produce a configure script.
AC_INIT([sidplayfp], [2.15.0], [], [], [https://github.com/libsidplayfp/sidplayfp/])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE



AC_CANONICAL_HOST
case "$host" in
    *mingw* )
        MINGW32=yes
        ;;
    *darwin* )
        MACOSX=yes
        ;;
esac
AM_CONDITIONAL([MINGW32], [test "x$MINGW32" = "xyes"])


AC_PROG_RANLIB

dnl Checks for programs.
AC_PROG_CXX

dnl Use C++ for tests.
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX([20], [noext], [optional])
AS_IF([test $ax_cv_cxx_compile_cxx20__std_cpp20 != "yes"], [
    AX_CXX_COMPILE_STDCXX([17], [noext], [optional])
    AS_IF([test $ax_cv_cxx_compile_cxx17__std_cpp17 != "yes"], [
        AX_CXX_COMPILE_STDCXX([14], [noext], [optional])
        AS_IF([test $ax_cv_cxx_compile_cxx14__std_cpp14 != "yes"],
            [AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])]
        )
    ])
])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN

AM_ICONV
AM_CONDITIONAL([USE_ICONV], [test "x$am_cv_func_iconv" = "xyes"])

dnl Audio subsystem

AUDIO_LDFLAGS=""

PKG_CHECK_MODULES(ALSA,
    [alsa >= 1.0],
    [AC_DEFINE([HAVE_ALSA], 1, [Define to 1 if you have libasound (-lasound).])],
    [AC_MSG_WARN([$ALSA_PKG_ERRORS])]
)

PKG_CHECK_MODULES(PULSE,
    [libpulse-simple >= 1.0],
    [AC_DEFINE([HAVE_PULSE], 1, [Define to 1 if you have libpulse-simple (-lpulse-simple).])],
    [AC_MSG_WARN([$PULSE_PKG_ERRORS])]
)


dnl Checks what version of Unix we have and soundcard support
AC_CHECK_HEADERS([sys/ioctl.h linux/soundcard.h machine/soundcard.h sys/soundcard.h soundcard.h])

AC_CHECK_HEADERS([dsound.h mmsystem.h], [], [], [#include <windows.h>])

AS_IF([test "$ac_cv_header_dsound_h" = "yes"],
    [AUDIO_LDFLAGS="$AUDIO_LDFLAGS -ldsound -ldxguid"]
)

AS_IF([test "$ac_cv_header_mmsystem_h" = "yes"],
    [AUDIO_LDFLAGS="$AUDIO_LDFLAGS -lwinmm"]
)

# NetBSD/OpenBSD OSS audio emulation
AS_IF([test "x$ac_cv_header_soundcard_h" = "xyes"],
    [AUDIO_LDFLAGS="$AUDIO_LDFLAGS -lossaudio"]
)

AC_SUBST(AUDIO_LDFLAGS)

PKG_CHECK_MODULES(SIDPLAYFP, [libsidplayfp >= 2.0])
PKG_CHECK_MODULES(STILVIEW, [libstilview >= 1.0])

USE_LIBOUT123=no
AC_ARG_WITH([out123], AS_HELP_STRING([--with-out123], [Build with out123 library (default: enabled)]))

AS_IF([test "x$with_out123" != "xno"],
    [PKG_CHECK_MODULES([OUT123],
        [libout123 >= 1.0],
        [USE_LIBOUT123=yes
        AC_DEFINE([HAVE_OUT123], [1], [Use libout123])],
        [USE_LIBOUT123=no]
    )]
)

AM_CONDITIONAL([USE_LIBOUT123], [test "x$USE_LIBOUT123" = "xyes"])

# check for builders
PKG_CHECK_VAR([SIDPLAYPF_PREFIX], [libsidplayfp], [prefix])

AC_MSG_CHECKING([for $SIDPLAYPF_PREFIX/include/sidplayfp/builders/residfp.h])
AS_IF([test -f $SIDPLAYPF_PREFIX/include/sidplayfp/builders/residfp.h],
    [AC_DEFINE([HAVE_SIDPLAYFP_BUILDERS_RESIDFP_H], [1], [residfp builder])
    AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])]
)
AC_MSG_CHECKING([for $SIDPLAYPF_PREFIX/include/sidplayfp/builders/resid.h])
AS_IF([test -f $SIDPLAYPF_PREFIX/include/sidplayfp/builders/resid.h],
    [AC_DEFINE([HAVE_SIDPLAYFP_BUILDERS_RESID_H], [1], [resid builder])
    AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])]
)
AC_MSG_CHECKING([for $SIDPLAYPF_PREFIX/include/sidplayfp/builders/hardsid.h])
AS_IF([test -f $SIDPLAYPF_PREFIX/include/sidplayfp/builders/hardsid.h],
    [AC_DEFINE([HAVE_SIDPLAYFP_BUILDERS_HARDSID_H], [1], [hardsid builder])
    AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])]
)
AC_MSG_CHECKING([for $SIDPLAYPF_PREFIX/include/sidplayfp/builders/exsid.h])
AS_IF([test -f $SIDPLAYPF_PREFIX/include/sidplayfp/builders/exsid.h],
    [AC_DEFINE([HAVE_SIDPLAYFP_BUILDERS_EXSID_H], [1], [exsid builder])
    AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])]
)

AC_MSG_CHECKING([for debugging])
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug], [compile for debugging @<:@no/yes, default=no@:>@])],
 [], [enable_debug=no])

AS_IF([test "x$enable_debug" = "xno"],
  [AC_MSG_RESULT([Build without debugging messages]); debug_flags=-DNDEBUG]
)

AC_SUBST([debug_flags])

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT
