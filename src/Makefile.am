
SUBDIRS = audio

AM_CPPFLAGS = -I $(top_builddir) \
$(SIDPLAYFP_CFLAGS)

# The console user interface.

bin_PROGRAMS = sidplayfp

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

sidplayfp_LDADD = ./audio/null/libnull.a \
./audio/wav/libwav.a \
$(AUDIO_DRV) $(AUDIO_LDFLAGS) \
$(SIDPLAYFP_LIBS) \
$(BUILDERS_LDFLAGS)
# $(LIBLTDL_LDFLAGS)

# Remove bad default includes
DEFAULT_INCLUDES=
