
SUBDIRS = audio

AM_CPPFLAGS = \
-I $(top_builddir) \
-DPKGDATADIR="\"$(pkgdatadir)/\"" \
$(SIDPLAYFP_CFLAGS) \
$(STILVIEW_CFLAGS) \
$(ALSA_CFLAGS) \
$(PULSE_CFLAGS) \
@debug_flags@

# The console user interface.

bin_PROGRAMS = sidplayfp stilview

sidplayfp_SOURCES = \
IniConfig.cpp \
IniConfig.h \
args.cpp \
keyboard.cpp \
keyboard.h \
main.cpp \
menu.cpp \
player.cpp \
player.h \
utils.cpp \
utils.h \
ini/iniHandler.h \
ini/iniHandler.cpp \
ini/dataParser.h \
ini/sidfstream.h \
ini/types.h

sidplayfp_LDADD = \
./audio/libaudio.a \
./audio/null/libnull.a \
./audio/wav/libwav.a \
$(AUDIO_DRV) \
$(AUDIO_LDFLAGS) \
$(SIDPLAYFP_LIBS) \
$(BUILDERS_LDFLAGS) \
$(ALSA_LIBS) \
$(PULSE_LIBS)

stilview_SOURCES = \
stilview.cpp

stilview_LDADD = \
$(STILVIEW_LIBS)
