
SUBDIRS = audio

AM_CPPFLAGS = -DHAVE_UNIX -I $(top_builddir)/unix \
$(SIDPLAY2_CFLAGS) \
$(SIDUTILS_CFLAGS)

# The console user interface.

bin_PROGRAMS = sidplay2

sidplay2_SOURCES = IniConfig.cpp IniConfig.h \
args.cpp keyboard.cpp keyboard.h main.cpp menu.cpp \
player.cpp player.h

sidplay2_LDADD = ./audio/null/libnull.a \
./audio/wav/libwav.a \
$(AUDIO_DRV) $(AUDIO_LDFLAGS) \
$(SIDPLAY2_LIBS) \
$(SIDUTILS_LIBS) \
$(BUILDERS_LDFLAGS)
# $(LIBLTDL_LDFLAGS)

# Remove bad default includes
DEFAULT_INCLUDES=
