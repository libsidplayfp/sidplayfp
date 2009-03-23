# Microsoft Developer Studio Project File - Name="sidplay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sidplay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sidplay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sidplay.mak" CFG="sidplay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sidplay - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sidplay - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sidplay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../include" /I "../../../libsidplay/include" /I "../../../libsidutils/include" /I "../../../builders/resid-builder/include" /I "../../../builders/hardsid-builder/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_MSWINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /subsystem:console /machine:I386 /out:"../../../bin_vc6/Release/sidplay2.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "sidplay - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sidplay_"
# PROP BASE Intermediate_Dir "sidplay_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../libsidplay/include" /I "../../../libsidutils/include" /I "../../../builders/resid-builder/include" /I "../../../builders/hardsid-builder/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_MSWINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib dxguid.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../bin_vc6/Debug/sidplay2.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sidplay - Win32 Release"
# Name "sidplay - Win32 Debug"
# Begin Group "Audio Drivers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\audio\AudioBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\AudioConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\alsa\audiodrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\AudioDrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\hpux\audiodrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\irix\audiodrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\oss\audiodrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\sunos\audiodrv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\directx\directx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\directx\directx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\mmsystem\mmsystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\mmsystem\mmsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\null\null.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\null\null.h
# End Source File
# End Group
# Begin Group "Audio Formats"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\audio\wav\WavFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\wav\WavFile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audio\wav\WavFileDefs.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\args.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\config.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\..\src\IniConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\IniConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\keyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\keyboard.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\menu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\player.h
# End Source File
# End Target
# End Project
