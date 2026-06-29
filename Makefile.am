
ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign subdir-objects

if MINGW32
  W32_CPPFLAGS = -DWIN32_LEAN_AND_MEAN
  W32_LIBS = -lshlwapi
endif

AM_CPPFLAGS = \
-I $(top_builddir)/src \
-I $(top_srcdir)/src \
-I $(top_srcdir)/libs/miniaudio \
-I $(top_srcdir)/libs \
-DPKGDATADIR="\"$(pkgdatadir)/\"" \
$(SIDPLAYFP_CFLAGS) \
$(STILVIEW_CFLAGS) \
${W32_CPPFLAGS} \
@debug_flags@


bin_PROGRAMS = \
src/sidplayfp \
src/stilview

#=========================================================
#fmt
if USE_SYSTEM_FMT
AM_CPPFLAGS += $(FMT_CFLAGS)
fmt_SOURCES =
else
AM_CPPFLAGS += -I $(top_srcdir)/libs/fmt_internal
fmt_SOURCES = \
libs/fmt_internal/fmt/args.h \
libs/fmt_internal/fmt/base.h \
libs/fmt_internal/fmt/chrono.h \
libs/fmt_internal/fmt/color.h \
libs/fmt_internal/fmt/compile.h \
libs/fmt_internal/fmt/core.h \
libs/fmt_internal/fmt/format-inl.h \
libs/fmt_internal/fmt/format.cc \
libs/fmt_internal/fmt/format.h \
libs/fmt_internal/fmt/os.cc \
libs/fmt_internal/fmt/os.h \
libs/fmt_internal/fmt/ostream.h \
libs/fmt_internal/fmt/printf.h \
libs/fmt_internal/fmt/ranges.h \
libs/fmt_internal/fmt/std.h \
libs/fmt_internal/fmt/xchar.h
endif

#=========================================================
# sidplayfp

src_sidplayfp_SOURCES = \
$(fmt_SOURCES) \
libs/miniaudio/osaudio_miniaudio.c \
libs/miniaudio/osaudio.h \
libs/miniaudio/miniaudio.h \
libs/filesystem/filesystem.hpp \
src/IniConfig.cpp \
src/IniConfig.h \
src/args.cpp \
src/dataParser.h \
src/keyboard.cpp \
src/keyboard.h \
src/main.cpp \
src/menu.cpp \
src/mixer.cpp \
src/mixer.h \
src/player.cpp \
src/player.h \
src/setting.h \
src/sidcxx11.h \
src/siddefines.h \
src/sidlib_features.h \
src/utils.cpp \
src/utils.h \
src/codeConvert.cpp \
src/codeConvert.h \
src/audio/AudioBase.h \
src/audio/AudioConfig.h \
src/audio/AudioDrv.cpp \
src/audio/AudioDrv.h \
src/audio/IAudio.h \
src/audio/au/auFile.cpp \
src/audio/au/auFile.h \
src/audio/miniaudio/audiodrv.cpp \
src/audio/miniaudio/audiodrv.h \
src/audio/null/null.cpp \
src/audio/null/null.h \
src/audio/wav/WavFile.cpp \
src/audio/wav/WavFile.h \
src/ini/iniHandler.h \
src/ini/iniHandler.cpp

src_sidplayfp_CXXFLAGS = \
$(PTHREAD_CFLAGS)

src_sidplayfp_LDADD = \
$(SIDPLAYFP_LIBS) \
$(W32_LIBS) \
$(PTHREAD_LIBS) \
$(FMT_LIBS)

#=========================================================
# stilview

src_stilview_SOURCES = \
$(fmt_SOURCES) \
src/codeConvert.cpp \
src/codeConvert.h \
src/stilview.cpp

src_stilview_LDADD = \
$(STILVIEW_LIBS) \
$(FMT_LIBS)

#=========================================================
# docs

EXTRA_DIST =  \
doc/en/sidplayfp.pod \
doc/en/sidplayfp.ini.pod \
doc/en/stilview.pod

dist_man_MANS = \
doc/en/sidplayfp.1 \
doc/en/sidplayfp.ini.5 \
doc/en/stilview.1

DISTCLEANFILES = $(dist_man_MANS)

.pod.1:
	@mkdir -p $(@D)
	pod2man -c "User Programs" -s 1 $< > $@

.pod.5:
	@mkdir -p $(@D)
	pod2man -c "File formats" -s 5 $< > $@
