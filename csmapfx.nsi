; csmapfx.nsi
;--------------------------------

; The name of the installer
Name "CsMapFX"

; The file to write
OutFile "..\csmapinst.exe"

; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

; The default installation directory
InstallDir $PROGRAMFILES\CsMapFX

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
Section "CsMapFX (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put files there
  File Release\CsMapFX.exe
  File Release\*.dll
  
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
  SetOutPath $INSTDIR
  
  ; Put file there
  File csmapfx.Eressea.info
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\CsMapFX"
  CreateShortcut "$SMPROGRAMS\CsMapFX\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortcut "$SMPROGRAMS\CsMapFX\CsMapFX.lnk" "$INSTDIR\CsMapFX.exe"

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CsMapFX"
  DeleteRegKey HKLM SOFTWARE\CsMapFX

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
