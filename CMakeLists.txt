cmake_minimum_required(VERSION 3.12)
project(sidplayfp VERSION 3.0.0 HOMEPAGE_URL https://github.com/libsidplayfp/sidplayfp/)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(HAVE_CXX11 1) # TODO remove this

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options("$<$<CONFIG:DEBUG>:-Wall;-Wextra;-pedantic>")
ENDIF()

# stupid cmake doesn't support CPPFLAGS
# append it to cmake flags so the headers can be found
# even if installed in a non standard dir
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} $ENV{CPPFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{CPPFLAGS}")

if (WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

set(SIDPLAYFP_SOURCES
    src/IniConfig.cpp
    src/IniConfig.h
    src/args.cpp
    src/keyboard.cpp
    src/keyboard.h
    src/main.cpp
    src/menu.cpp
    src/player.cpp
    src/player.h
    src/sidcxx11.h
    src/sidlib_features.h
    src/utils.cpp
    src/utils.h
    src/codeConvert.cpp
    src/codeConvert.h
    src/audio/AudioBase.h
    src/audio/AudioConfig.h
    src/audio/AudioDrv.cpp
    src/audio/AudioDrv.h
    src/audio/IAudio.h
    src/audio/alsa/audiodrv.cpp
    src/audio/alsa/audiodrv.h
    src/audio/au/auFile.cpp
    src/audio/au/auFile.h
    src/audio/directx/audiodrv.cpp
    src/audio/directx/audiodrv.h
    src/audio/mmsystem/audiodrv.cpp
    src/audio/mmsystem/audiodrv.h
    src/audio/null/null.cpp
    src/audio/null/null.h
    src/audio/oss/audiodrv.cpp
    src/audio/oss/audiodrv.h
    src/audio/pulse/audiodrv.cpp
    src/audio/pulse/audiodrv.h
    src/audio/wav/WavFile.cpp
    src/audio/wav/WavFile.h
    src/ini/iniHandler.h
    src/ini/iniHandler.cpp
    src/ini/dataParser.h
    src/ini/sidfstream.h
    src/ini/types.h
)

set(STILVIEW_SOURCES
    src/stilview.cpp
)

#####################################################

include(GNUInstallDirs)

set(top_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")

set(VERSION "${CMAKE_PROJECT_VERSION}")
set(PACKAGE_URL "${PROJECT_HOMEPAGE_URL}")
add_compile_definitions(PKGDATADIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")

#####################################################

add_executable(sidplayfp ${SIDPLAYFP_SOURCES})
add_executable(stilview ${STILVIEW_SOURCES})

target_include_directories(sidplayfp PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(sidplayfp PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

find_package(PkgConfig REQUIRED)

INSTALL(TARGETS sidplayfp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(TARGETS stilview RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

#####################################################
INCLUDE (TestBigEndian)

test_big_endian(IS_BIGENDIAN)
if(IS_BIGENDIAN)
    add_compile_definitions(WORDS_BIGENDIAN)
endif(IS_BIGENDIAN)

#####################################################
# Check for iconv

find_package(Iconv)
if (Iconv_FOUND)
    SET(HAVE_ICONV 1)
    list(APPEND SIDPLAYFP_SOURCES
        src/codepages.h
    )
    target_link_libraries(sidplayfp Iconv::Iconv)
endif(Iconv_FOUND)

#####################################################
# Check for ALSA

find_package(ALSA)
if (ALSA_FOUND)
    SET(HAVE_ALSA 1)
    target_link_libraries(sidplayfp ALSA::ALSA)
endif(ALSA_FOUND)

#####################################################
INCLUDE (CheckIncludeFiles)

if(WIN32)
    CHECK_INCLUDE_FILES("windows.h;mmsystem.h" HAVE_MMSYSTEM_H)
    if(HAVE_MMSYSTEM_H)
        target_link_libraries(sidplayfp winmm)

        CHECK_INCLUDE_FILES("windows.h;dsound.h" HAVE_DSOUND_H)
        if(HAVE_DSOUND_H)
            target_link_libraries(sidplayfp dsound dxguid)
        endif()
    endif()
else()
    CHECK_INCLUDE_FILES("sys/soundcard.h" HAVE_SYS_SOUNDCARD_H)
    CHECK_INCLUDE_FILES("linux/soundcard.h" HAVE_LINUX_SOUNDCARD_H)
    CHECK_INCLUDE_FILES("machine/soundcard.h" HAVE_MACHINE_SOUNDCARD_H)
    CHECK_INCLUDE_FILES("soundcard.h" HAVE_SOUNDCARD_H)
    if(HAVE_SOUNDCARD_H)
        target_link_libraries(sidplayfp ossaudio)
    endif()
endif()


#####################################################
# Check for libout123

include(CMakeDependentOption)

# enable ou123 by default only on Mac OSX since we don't have any audio backend there
CMAKE_DEPENDENT_OPTION(OUT123 "libout123 support" ON APPLE OFF)
if(OUT123)
    pkg_check_modules(libout123 IMPORTED_TARGET libout123>=1.0)
    if (libout123_FOUND)
        SET(HAVE_OUT123 1)
        target_link_libraries(sidplayfp PkgConfig::libout123)
    endif(libout123_FOUND)
endif(OUT123)

##################################################################
# Search for libsidplayfp

pkg_check_modules(libsidplayfp IMPORTED_TARGET libsidplayfp>=1.0)
if(libsidplayfp_FOUND)
    SET(HAVE_SIDPLAYFP 1)
    target_link_libraries(sidplayfp PkgConfig::libsidplayfp)

    INCLUDE(CheckIncludeFileCXX)
    LIST(APPEND CMAKE_REQUIRED_INCLUDES ${libsidplayfp_INCLUDE_DIRS})
    CHECK_INCLUDE_FILE_CXX(sidplayfp/builders/residfp.h HAVE_SIDPLAYFP_BUILDERS_RESIDFP_H)
    CHECK_INCLUDE_FILE_CXX(sidplayfp/builders/resid.h HAVE_SIDPLAYFP_BUILDERS_RESID_H)
    CHECK_INCLUDE_FILE_CXX(sidplayfp/builders/hardsid.h HAVE_SIDPLAYFP_BUILDERS_HARDSID_H)
    CHECK_INCLUDE_FILE_CXX(sidplayfp/builders/exsid.h HAVE_SIDPLAYFP_BUILDERS_EXSID_H)
endif(libsidplayfp_FOUND)

##################################################################
# Check for STIL

if(libsidplayfp_FOUND)
    pkg_check_modules(libstilview IMPORTED_TARGET libstilview>=1.0)
    if(libstilview_FOUND)
        SET(HAVE_STILVIEW 1)
        target_link_libraries(stilview PkgConfig::libstilview)
    endif(libstilview_FOUND)
endif()

##################################################################

if (WIN32)
    target_link_libraries(sidplayfp shlwapi)
endif()

##################################################################

find_package(Perl)
if (PERL_FOUND)
    get_filename_component(PERL_DIR ${PERL_EXECUTABLE} DIRECTORY)
    find_program(POD2MAN pod2man HINTS ${PERL_DIR})

    if(POD2MAN)
        message(STATUS "Found pod2man - man pages enabled")
        set(MAN_NAMES
            doc/en/sidplayfp.1
            doc/en/sidplayfp.ini.5
            doc/en/stilview.1
        )
        set(MAN_FILES)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/doc/en")
        foreach(MANNAME IN LISTS MAN_NAMES)
            set(MANFILE ${CMAKE_BINARY_DIR}/${MANNAME})
            string(REGEX REPLACE "\.[0-9]$" ".pod" PODNAME ${MANNAME})
            set(PODFILE ${CMAKE_SOURCE_DIR}/${PODNAME})
            string(REGEX MATCH "[0-9]$" SECTION ${MANNAME})
            if(${SECTION} STREQUAL "1")
                set(CENTER "User Programs")
            elseif(${SECTION} STREQUAL "5")
                set(CENTER "File formats")
            endif()
            add_custom_command(
                OUTPUT ${MANFILE}
                COMMAND ${PERL_EXECUTABLE} ${POD2MAN}
                ARGS -c ${CENTER} -s ${SECTION} ${PODFILE} > ${MANFILE}
                DEPENDS ${PODFILE}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Building manpage ${MANFILE}"
                VERBATIM
            )
            list(APPEND MAN_FILES ${MANFILE})
        endforeach()

        add_custom_target(man ALL DEPENDS ${MAN_FILES})
    else()
        message(STATUS "Could not find pod2man - man pages disabled")
    endif(POD2MAN)
endif()

##################################################################

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.template ${CMAKE_CURRENT_BINARY_DIR}/src/config.h @ONLY)

add_compile_definitions(HAVE_CONFIG_H)
