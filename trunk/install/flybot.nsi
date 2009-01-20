;NSIS Modern User Interface
!define VERSION "0.22.3"

!ifndef VERSION
  !define VERSION 'anonymous-build'
!endif

;--------------------------------
;Configuration

!ifdef OUTFILE
  OutFile "${OUTFILE}"
!else
  OutFile .\flybot-${VERSION}.exe
!endif

SetCompressor /SOLID lzma

;RequestExecutionLevel admin
;--------------------------------
;Include Modern UI 2

  !include "MUI2.nsh"
  !include "InstallOptions.nsh"
;--------------------------------
;General

  ;Name and file
  Name "flybot"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\FlylinkDC++"
  
  DirText "��������� ��������� $(^NameDA) � ����� �������. ����� ������� ������ �����, ������� ������ '�����' � ������� ��."
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\FlylinkDC++_is1 "InstallLocation"

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  ;!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
  !define MUI_HEADERIMAGE_BITMAP "..\ico\fly\fly.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  ;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  ;!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  ;!define MUI_FINISHPAGE_LINK "Visit the NSIS site for the latest news, FAQs and support"
  ;!define MUI_FINISHPAGE_LINK_LOCATION "http://flybot.googlecode.com"
  ;!insertmacro MUI_PAGE_FINISH
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Russian"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy
  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File ..\bin\ChatBot.dll
  SetOutPath "$INSTDIR\Settings"
  SetOverwrite off
  File ..\bin\Settings\flydict.ini
  
  ;Store installation folder
  WriteRegStr HKCU "Software\flybot" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall-flybot.exe"
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  ;LangString DirText ${LANG_RUSSIAN} "A test section."

  ;Assign language strings to sections
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    ;!insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} DirText
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\ChatBot.dll"
  Delete "$INSTDIR\uninstall-flybot.exe"

  DeleteRegKey /ifempty HKCU "Software\flybot"

SectionEnd