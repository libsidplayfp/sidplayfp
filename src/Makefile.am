
SUBDIRS = audio

AM_CPPFLAGS = \
-I $(top_builddir) \
-DPKGDATADIR="\"$(pkgdatadir)/\"" \
$(SIDPLAYFP_CFLAGS) \
$(STILVIEW_CFLAGS) \
$(ALSA_CFLAGS) \
$(PULSE_CFLAGS)

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
ini/libini.h \
ini/headings.h \
ini/headings.i \
ini/ini.cpp \
ini/ini.h \
ini/keys.h \
ini/keys.i \
ini/list.h \
ini/list.i \
ini/types.i

sidplayfp_LDADD = \
./audio/libaudio.a \
./audio/null/libnull.a \
./audio/wav/libwav.a \
$(AUDIO_DRV) $(AUDIO_LDFLAGS) \
$(SIDPLAYFP_LIBS) \
$(BUILDERS_LDFLAGS) \
$(ALSA_LIBS) \
$(PULSE_LIBS)
# $(LIBLTDL_LDFLAGS)

stilview_SOURCES = \
stilview.cpp

stilview_LDADD = \
$(STILVIEW_LIBS)

# Remove bad default includes
DEFAULT_INCLUDES=
