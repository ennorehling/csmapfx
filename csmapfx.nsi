; csmapfx.nsi
;--------------------------------

; The name of the installer
Name "CsMapFX"

!searchparse /file version.h `#define CSMAP_APP_VERSIONSTR "v` APP_MAJOR `.` APP_MINOR `.` APP_PATCHSTR `"`

!define MAIN_SECTION "CsMapFX ${APP_VERSION}"
!define APP_VERSION "${APP_MAJOR}.${APP_MINOR}.${APP_PATCHSTR}"

!getdllversion "Release\CsMapFX.exe" expv_
!define PROD_VERSION "${expv_1}.${expv_2}.${expv_3}.${expv_4}"
VIProductVersion ${PROD_VERSION}

; The file to write
OutFile "..\csmapinst-${APP_VERSION}.exe"
VIAddVersionKey /LANG=0 "CompanyName" "Eressea"
VIAddVersionKey /LANG=0 "ProductName" "Coast and Sea Mapper"
VIAddVersionKey /LANG=0 "FileDescription" "CsMapFX Application"
VIAddVersionKey /LANG=0 "FileVersion" ${APP_VERSION}
VIAddVersionKey /LANG=0 "LegalCopyright" "Enno Rehling"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

; The default installation directory
InstallDir $PROGRAMFILES\Eressea\CsMapFX

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\CsMapFX" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "${MAIN_SECTION} (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put files there
  File Release\CsMapFX.exe
  ; File Release\*.dll
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\CsMapFX "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX" "DisplayName" "CsMapFX"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

Section "Additional Game Information"

  ; Set output path to the installation directory.
  SetOutPath $LOCALAPPDATA\Eressea\CsMapFX
  
  ; Put file there
  File csmapfx.Eressea.info
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\CsMapFX"
  CreateShortcut "$SMPROGRAMS\CsMapFX\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortcut "$SMPROGRAMS\CsMapFX\CsMapFX.lnk" "$INSTDIR\CsMapFX.exe"

SectionEnd

Section "Register File Types"
WriteRegStr HKCU 'Software\Classes\.cr\OpenWithProgIDs' 'Eressea.Tools.CsMapFX' ''
WriteRegStr HKCU 'Software\Classes\.cr\DefaultIcon' '' '$INSTDIR\CsMapFX.exe'
WriteRegStr HKCU 'Software\Classes\Eressea.Tools.CsMapFX\shell\open\command' '' '"$INSTDIR\CsMapFX.exe" "%1"'
SectionEnd
;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX"
  DeleteRegKey HKLM SOFTWARE\CsMapFX
  DeleteRegKey HKCU 'Software\Classes\.cr'
  DeleteRegKey HKCU 'Software\Classes\Eressea.Tools.CsMapFX'

  ; Remove files and uninstaller
  Delete $INSTDIR\CsMapFX.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.info
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\CsMapFX\*.lnk"

  ; Remove directories
  RMDir "$SMPROGRAMS\CsMapFX"
  RMDir "$INSTDIR"

SectionEnd
