# Microsoft Developer Studio Project File - Name="CerfPDASimulator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CerfPDASimulator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CerfPDASimulator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CerfPDASimulator.mak" CFG="CerfPDASimulator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CerfPDASimulator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CerfPDASimulator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CerfPDASimulator - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "C:\Cygwin\home\Software\uC-GUI\Start\gui\Core" /I "C:\Cygwin\home\Software\uC-GUI\Start\gui\Font" /I "C:\Cygwin\home\Software\uC-GUI\Start\gui\Widget" /I "C:\Cygwin\home\Software\uC-GUI\Start\gui\WM" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0c /d "NDEBUG"
# ADD RSC /l 0xc0c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib libc.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib

!ELSEIF  "$(CFG)" == "CerfPDASimulator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "..\..\..\..\..\..\uC-GUI\Start\gui\Core" /I "..\..\..\..\..\..\uC-GUI\Start\gui\Font" /I "..\..\..\..\..\..\uC-GUI\Start\gui\Widget" /I "..\..\..\..\..\..\uC-GUI\Start\gui\WM" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0c /d "_DEBUG"
# ADD RSC /l 0xc0c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib libc.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CerfPDASimulator - Win32 Release"
# Name "CerfPDASimulator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Sample\Application\HouseControl.c"
# End Source File
# Begin Source File

SOURCE=.\Main.c
# End Source File
# Begin Source File

SOURCE=.\SIM_X.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\BUTTON.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\BUTTON_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\CHECKBOX.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\DIALOG.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\DIALOG_Intern.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\DROPDOWN.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\EDIT.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\EDIT_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\FRAMEWIN.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\FRAMEWIN_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUI.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\GUI_ARRAY.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUI_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUI_Protected.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUI_X.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUIDebug.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUIType.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\GUIVersion.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\HEADER.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\HEADER_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jconfig.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jdct.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jdhuff.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jerror.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jinclude.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jmemsys.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jmorecfg.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jpegint.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jpeglib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\JPEG\jversion.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCD.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCD_ConfDefaults.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCD_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCD_Protected.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCDFixedPalette.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Core\LCDSIM.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\LISTBOX.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\LISTBOX_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\LISTVIEW.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\MARQUEE.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\MULTIEDIT.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\MULTIPAGE.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\PROGBAR.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\RADIO.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\SCROLLBAR.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\SCROLLBAR_Private.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\SLIDER.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\TEXT.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\Widget\WIDGET.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\WM\WM.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\WM\WM_GUI.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\WM\WM_Intern.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\uC-GUI\Start\gui\WM\WM_Intern_ConfDep.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\GUIConf.h
# End Source File
# Begin Source File

SOURCE=.\GUITouchConf.h
# End Source File
# Begin Source File

SOURCE=.\LCDConf.h
# End Source File
# Begin Source File

SOURCE=.\SIM.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\Branding.rh
# End Source File
# Begin Source File

SOURCE=.\Res\Device.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Device1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Logo.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ResourceSim.h
# End Source File
# Begin Source File

SOURCE=.\Res\Simulation.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Simulation.rc
# End Source File
# End Group
# Begin Group "Library Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Lib\Release\GUI.lib
# End Source File
# Begin Source File

SOURCE=.\GUISim.lib
# End Source File
# End Group
# End Target
# End Project
