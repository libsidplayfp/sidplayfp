dnl Process this file with autoconf to produce a configure script.
AC_INIT([sidplayfp], [1.2.1], [], [], [http://sourceforge.net/projects/sidplay-residfp/])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE


AC_CANONICAL_HOST

AC_PROG_RANLIB

dnl Checks for programs.
AC_PROG_CXX

dnl Use C++ for tests.
AC_LANG([C++])


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN


dnl Audio subsystem

audiodrv_libadd=""
AUDIO_LDFLAGS=""

PKG_CHECK_MODULES(ALSA,
    [alsa >= 1.0],
    [AC_DEFINE([HAVE_ALSA], 1, [Define to 1 if you have libasound (-lasound).])
    audiodrv_libadd="$audiodrv_libadd ./audio/alsa/libalsa.a"],
    [AC_MSG_WARN([$ALSA_PKG_ERRORS])]
)

PKG_CHECK_MODULES(PULSE,
    [libpulse-simple >= 1.0],
    [AC_DEFINE([HAVE_PULSE], 1, [Define to 1 if you have libpulse-simple (-lpulse-simple).])
    audiodrv_libadd="$audiodrv_libadd ./audio/pulse/libpulse.a"],
    [AC_MSG_WARN([$PULSE_PKG_ERRORS])]
)


dnl Checks what version of Unix we have and soundcard support
AC_CHECK_HEADERS([sys/ioctl.h linux/soundcard.h machine/soundcard.h \
sys/soundcard.h soundcard.h])

AC_CHECK_HEADERS([dsound.h mmsystem.h], [], [], [#include <windows.h>])

AS_IF([test "$ac_cv_header_dsound_h" = "yes"],
    [audiodrv_libadd="$audiodrv_libadd ./audio/directx/libdirectx.a"
    AUDIO_LDFLAGS="$AUDIO_LDFLAGS -ldsound -ldxguid"]
)

AS_IF([test "$ac_cv_header_mmsystem_h" = "yes"],
    [audiodrv_libadd="$audiodrv_libadd ./audio/mmsystem/libmmsystem.a"
    AUDIO_LDFLAGS="$AUDIO_LDFLAGS -lwinmm"]
)

# NetBSD/OpenBSD OSS audio emulation
AS_IF([test "x$ac_cv_header_soundcard_h" = "xyes"],
    [audiodrv_libadd="$audiodrv_libadd ./audio/oss/liboss.a"
    AUDIO_LDFLAGS="$AUDIO_LDFLAGS -lossaudio"]
)

AS_IF([test "x$ac_cv_header_machine_soundcard_h" = "xyes" -o "x$ac_cv_header_linux_soundcard_h" = "xyes" -o "x$ac_cv_header_sys_soundcard_h" = "xyes"],
    [audiodrv_libadd="$audiodrv_libadd ./audio/oss/liboss.a"]
)

AC_SUBST(AUDIO_LDFLAGS)

AUDIO_DRV="$audiodrv_libadd"
AC_SUBST(AUDIO_DRV)

AC_CHECK_FUNCS([strncasecmp strcasecmp])

PKG_CHECK_MODULES(SIDPLAYFP,[libsidplayfp >= 1.0])
PKG_CHECK_MODULES(STILVIEW,[libstilview >= 1.0])

# hack?
saveCPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $SIDPLAYFP_CFLAGS"

AC_CHECK_HEADERS([sidplayfp/builders/residfp.h sidplayfp/builders/resid.h sidplayfp/builders/hardsid.h])

CPPFLAGS=$saveCPPFLAGS


AC_CONFIG_FILES([
Makefile
doc/Makefile
doc/en/Makefile
src/Makefile
src/audio/Makefile
src/audio/alsa/Makefile
src/audio/directx/Makefile
src/audio/mmsystem/Makefile
src/audio/null/Makefile
src/audio/oss/Makefile
src/audio/pulse/Makefile
src/audio/wav/Makefile
])

AC_OUTPUT
