dnl Process this file with autoconf to produce a configure script.
AC_INIT([sidplayfp], [1.1.0alpha], [], [], [http://sourceforge.net/projects/sidplay-residfp/])
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

dnl Checks what version of Unix we have and soundcard support
AC_CHECK_HEADERS([sys/ioctl.h linux/soundcard.h machine/soundcard.h  \
soundcard.h sys/audio.h sun/audioio.h sun/dbriio.h sys/audioio.h  \
audio.h dmedia/audio.h sys/soundcard.h sys/asoundlib.h alsa/asoundlib.h \
pulse/simple.h dsound.h])

AH_TOP(
[/* Define supported audio driver */
#undef HAVE_HARDSID
#undef HAVE_ALSA
#undef HAVE_DIRECTX
#undef HAVE_MMSYSTEM
#undef HAVE_OSS
#undef HAVE_PULSE
])

dnl Audio subsystem
AC_ARG_ENABLE(driver,
    [AS_HELP_STRING(
	    [--enable-driver=pulse|alsa|oss|directx|mmsystem|wav],
	    [which audio driver to use (default is autodetect)])],
    [
    ],
    [enable_driver=autodetect])

if test "$enable_driver" == "autodetect"; then
    AC_MSG_CHECKING([which audio subsystem to use])
    case "$host" in
	*netbsd*|*freebsd*)
	    enable_driver=oss;;
	*qnx*)
	    enable_driver=alsa;;
	*cygwin*)
	    enable_driver=mmsystem;;
	*linux*)
	    if test "$ac_cv_header_pulse_simple_h" == "yes"; then
		enable_driver=pulse
            elif test "$ac_cv_header_sys_asoundlib_h" == "yes" ||
                test "$ac_cv_header_alsa_asoundlib_h" == "yes"; then
		enable_driver=alsa
            else
		enable_driver=oss
	    fi
	    ;;
	*mingw*)
	    if test "$ac_cv_header_dsound_h" == "yes"; then
		enable_driver=directx
            else
		enable_driver=mmsystem
            fi
	    ;;
	*)
	    enable_driver="wav"
	    ;;
    esac
    AC_MSG_RESULT($enable_driver (autodetected, use --enable-driver to override))
fi

case $enable_driver in
    pulse)
        AC_DEFINE(HAVE_PULSE)
        audiodrv_libadd="./audio/pulse/libpulse.a"
        AC_CHECK_LIB(pulse, main, [AUDIO_LDFLAGS=-lpulse-simple AC_SUBST(AUDIO_LDFLAGS)], [exit 1])
	;;
    alsa)
        AC_DEFINE(HAVE_ALSA)
        audiodrv_libadd="./audio/alsa/libalsa.a"
        AC_CHECK_LIB(asound, main, [AUDIO_LDFLAGS=-lasound AC_SUBST(AUDIO_LDFLAGS)], [exit 1])
	;;
    oss)
	AC_DEFINE(HAVE_OSS)
        audiodrv_libadd="./audio/oss/liboss.a"
	AC_CHECK_LIB(ossaudio, main, [AUDIO_LDFLAGS=-lossaudio AC_SUBST(AUDIO_LDFLAGS)], [exit 1])
	;;
    directx)
        AC_DEFINE(HAVE_DIRECTX)
        audiodrv_libadd="./audio/directx/libdirectx.a"
        AC_MSG_RESULT(directx)
        AUDIO_LDFLAGS="-ldsound -ldxguid"
	;;
    mmsystem)
	AC_DEFINE(HAVE_MMSYSTEM)
	audiodrv_libadd="./audio/mmsystem/libmmsystem.a"
	AC_MSG_RESULT(mmsystem)
	AUDIO_LDFLAGS=-lwinmm
	;;
esac

AUDIO_DRV="$audiodrv_libadd"
AC_SUBST(AUDIO_DRV)

dnl Check for nothrow allocator.
MY_CHECK_EXCEPTIONS

dnl Check ANSI C++ ios features
MY_CHECK_IOS_BIN
MY_CHECK_IOS_OPENMODE

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
