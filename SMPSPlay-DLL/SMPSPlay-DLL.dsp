# Microsoft Developer Studio Project File - Name="SMPSOUT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SMPSOUT - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "SMPSOUT.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "SMPSOUT.mak" CFG="SMPSOUT - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "SMPSOUT - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SMPSOUT - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SMPSOUT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release_VC6"
# PROP Intermediate_Dir "Release_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SMPSOUT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\SMPSPlay\libs\include" /I "..\SMPSPlay\libs\install\include" /D "ENABLE_LOOP_DETECTION" /D "DISABLE_DLOAD_FILE" /D "DISABLE_DEBUG_MSGS" /D "DISABLE_NECPCM" /D "AUDDRV_WAVEWRITE" /D "AUDDRV_WINMM" /D "AUDDRV_DSOUND" /D "AUDDRV_XAUD2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SMPSOUT_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib ole32.lib winmm.lib dsound.lib vgm-audio_VC6.lib vgm-emu_VC6.lib /nologo /dll /machine:I386 /libpath:"..\SMPSPlay\libs\install\lib"

!ELSEIF  "$(CFG)" == "SMPSOUT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug_VC6"
# PROP Intermediate_Dir "Debug_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SMPSOUT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\SMPSPlay\libs\include" /I "..\SMPSPlay\libs\install\include" /D "ENABLE_LOOP_DETECTION" /D "DISABLE_DLOAD_FILE" /D "DISABLE_DEBUG_MSGS" /D "DISABLE_NECPCM" /D "AUDDRV_WAVEWRITE" /D "AUDDRV_WINMM" /D "AUDDRV_DSOUND" /D "AUDDRV_XAUD2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SMPSOUT_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib ole32.lib winmm.lib dsound.lib vgm-audio_VC6.lib vgm-emu_VC6.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\SMPSPlay\libs\install\lib"

!ENDIF 

# Begin Target

# Name "SMPSOUT - Win32 Release"
# Name "SMPSOUT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SMPSOUT.cpp
# End Source File
# Begin Source File

SOURCE=.\songinfo.gen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\musicid.gen.h
# End Source File
# Begin Source File

SOURCE=.\resource.gen.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource_SKC.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SMPSOUT.gen.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SMPSOUT.rc
# End Source File
# End Group
# Begin Group "SMPSPlay Header Files"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\dac.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\loader.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\loader_data.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\loader_smps.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_commands.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_int.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_structs.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_structs_int.h
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Sound.h
# End Source File
# End Group
# Begin Group "SMPSPlay Source Files"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\dac.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\loader_data.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\loader_smps.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_commands.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_drums.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Engine\smps_extra.c
# End Source File
# Begin Source File

SOURCE=..\SMPSPlay\src\Sound.c
# End Source File
# End Group
# End Target
# End Project
