
AUTOMAKE_OPTIONS = foreign subdir-objects


AM_CPPFLAGS = \
-I $(top_builddir) \
-DPKGDATADIR="\"$(pkgdatadir)/\"" \
$(SIDPLAYFP_CFLAGS) \
$(STILVIEW_CFLAGS) \
$(ALSA_CFLAGS) \
$(PULSE_CFLAGS)


bin_PROGRAMS = \
src/sidplayfp \
src/stilview

#=========================================================
# sidplayfp

src_sidplayfp_SOURCES = \
src/IniConfig.cpp \
src/IniConfig.h \
src/args.cpp \
src/keyboard.cpp \
src/keyboard.h \
src/main.cpp \
src/menu.cpp \
src/player.cpp \
src/player.h \
src/utils.cpp \
src/utils.h \
src/audio/AudioBase.h \
src/audio/AudioConfig.h \
src/audio/AudioDrv.cpp \
src/audio/AudioDrv.h \
src/audio/IAudio.h \
src/audio/alsa/audiodrv.cpp \
src/audio/alsa/audiodrv.h \
src/audio/directx/audiodrv.cpp \
src/audio/directx/audiodrv.h \
src/audio/mmsystem/audiodrv.cpp \
src/audio/mmsystem/audiodrv.h \
src/audio/null/null.cpp \
src/audio/null/null.h \
src/audio/oss/audiodrv.cpp \
src/audio/oss/audiodrv.h \
src/audio/pulse/audiodrv.cpp \
src/audio/pulse/audiodrv.h \
src/audio/wav/WavFile.cpp \
src/audio/wav/WavFile.h \
src/ini/custom_locale.h \
src/ini/iniHandler.h \
src/ini/iniHandler.cpp \
src/ini/dataParser.h \
src/ini/sidfstream.h \
src/ini/types.h

src_sidplayfp_LDADD = \
$(AUDIO_LDFLAGS) \
$(SIDPLAYFP_LIBS) \
$(BUILDERS_LDFLAGS) \
$(ALSA_LIBS) \
$(PULSE_LIBS)

#=========================================================
#stilview

src_stilview_SOURCES = \
src/stilview.cpp

src_stilview_LDADD = \
$(STILVIEW_LIBS)


dist_man_MANS = \
doc/en/sidplayfp.1 \
doc/en/sidplayfp.ini.5 \
doc/en/stilview.1

