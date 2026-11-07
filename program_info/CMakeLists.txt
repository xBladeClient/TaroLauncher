if(UNIX)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_search_module(SCDOC scdoc)
        if(SCDOC_FOUND)
            pkg_get_variable(SCDOC_SCDOC scdoc scdoc)
        endif()
    endif()
endif()

set(Launcher_CommonName "TaroLauncher")
set(Launcher_DisplayName "TaroLauncher")
set(Launcher_AppID "com.tarolauncher.TaroLauncher")
set(Launcher_Domain "tarolauncher.com")
set(Launcher_Git "https://github.com/TaroLauncher/TaroLauncher")

set(Launcher_Name "${Launcher_CommonName}" PARENT_SCOPE)
set(Launcher_DisplayName "${Launcher_DisplayName}" PARENT_SCOPE)
set(Launcher_ENVName "TAROLAUNCHER" PARENT_SCOPE)
set(Launcher_Domain "${Launcher_Domain}" PARENT_SCOPE)
set(Launcher_Git "${Launcher_Git}" PARENT_SCOPE)

set(Launcher_SVGFileName "${Launcher_AppID}.svg")
set(Launcher_Copyright "© 2026 TaroLauncher\\nBasado en Prism Launcher (GPL-3.0): © 2022-2026 Prism Launcher Contributors, © 2021-2022 PolyMC Contributors, © 2012-2021 MultiMC Contributors")
set(Launcher_Copyright_Mac "© 2026 TaroLauncher. Basado en Prism Launcher (GPL-3.0)" PARENT_SCOPE)
set(Launcher_Copyright "${Launcher_Copyright}" PARENT_SCOPE)
set(Launcher_UserAgent "${Launcher_CommonName}/${Launcher_VERSION_NAME}" PARENT_SCOPE)
set(Launcher_ConfigFile "${Launcher_APP_BINARY_NAME}.cfg" PARENT_SCOPE)
set(Launcher_AppID "${Launcher_AppID}" PARENT_SCOPE)
set(Launcher_SVGFileName "${Launcher_SVGFileName}" PARENT_SCOPE)

set(Launcher_Desktop "program_info/${Launcher_AppID}.desktop" PARENT_SCOPE)
set(Launcher_MIMEInfo "program_info/${Launcher_AppID}.xml" PARENT_SCOPE)
set(Launcher_MetaInfo "program_info/${Launcher_AppID}.metainfo.xml" PARENT_SCOPE)
set(Launcher_PNG_256 "program_info/${Launcher_AppID}_256.png" PARENT_SCOPE)
set(Launcher_SVG "program_info/${Launcher_SVGFileName}" PARENT_SCOPE)
set(Launcher_Branding_ICNS "program_info/${Launcher_APP_BINARY_NAME}.icns" PARENT_SCOPE)
set(Launcher_Branding_MAC_ICON "program_info/${Launcher_CommonName}.icon" PARENT_SCOPE)
set(Launcher_Branding_ICO "program_info/${Launcher_APP_BINARY_NAME}.ico")
set(Launcher_Branding_ICO "${Launcher_Branding_ICO}" PARENT_SCOPE)
set(Launcher_Branding_WindowsRC "program_info/${Launcher_APP_BINARY_NAME}.rc" PARENT_SCOPE)
set(Launcher_Branding_LogoQRC "program_info/${Launcher_APP_BINARY_NAME}.qrc" PARENT_SCOPE)
set(Launcher_Authors "TaroLauncher")

set(Launcher_Portable_File "program_info/portable.txt" PARENT_SCOPE)

configure_file(${Launcher_AppID}.desktop.in ${Launcher_AppID}.desktop)
configure_file(${Launcher_AppID}.metainfo.xml.in ${Launcher_AppID}.metainfo.xml)
configure_file(${Launcher_APP_BINARY_NAME}.rc.in ${Launcher_APP_BINARY_NAME}.rc @ONLY)
configure_file(${Launcher_APP_BINARY_NAME}.qrc.in ${Launcher_APP_BINARY_NAME}.qrc @ONLY)
configure_file(${Launcher_APP_BINARY_NAME}.manifest.in ${Launcher_APP_BINARY_NAME}.manifest @ONLY)
configure_file(${Launcher_APP_BINARY_NAME}.ico ${Launcher_APP_BINARY_NAME}.ico COPYONLY)
configure_file(${Launcher_SVGFileName} ${Launcher_SVGFileName} COPYONLY)
configure_file(${Launcher_AppID}.mime.xml ${Launcher_AppID}.xml COPYONLY)

if(MSVC)
    set(Launcher_MSVC_Redist_NSIS_Section [=[
!ifdef haveNScurl
; Hidden section: the Microsoft Visual C++ runtime (NOT Visual Studio) is needed by tarolauncher.exe.
; It is only downloaded when it is not installed yet.
Section "-Microsoft Visual C++ Runtime"
    Var /GLOBAL vc_redist_exe
    Var /GLOBAL vc_installed
    ${If} ${IsNativeARM64}
        StrCpy $vc_redist_exe "vc_redist.arm64.exe"
        SetRegView 64
        ReadRegDWORD $vc_installed HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\arm64" "Installed"
    ${Else}
        StrCpy $vc_redist_exe "vc_redist.x64.exe"
        SetRegView 64
        ReadRegDWORD $vc_installed HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
    ${EndIf}
    SetRegView default
    ${If} $vc_installed == 1
        DetailPrint "Microsoft Visual C++ Runtime already installed"
        Return
    ${EndIf}
    DetailPrint 'Downloading Microsoft Visual C++ Redistributable...'
    NScurl::http GET "https://aka.ms/vs/17/release/$vc_redist_exe" "$INSTDIR\vc_redist\$vc_redist_exe" /INSIST /CANCEL /Zone.Identifier /END
    Pop $0
    ${If} $0 == "OK"
        DetailPrint "Download successful"
        ExecWait "$INSTDIR\vc_redist\$vc_redist_exe /install /passive /norestart"
    ${Else}
        DetailPrint "Download failed with error $0"
    ${EndIf}
SectionEnd
!endif
]=])
endif()

configure_file(win_install.nsi.in win_install.nsi @ONLY)

if(SCDOC_FOUND)
    configure_file(${Launcher_APP_BINARY_NAME}.6.scd.in ${Launcher_APP_BINARY_NAME}.6.scd @ONLY)

    set(in_scd "${CMAKE_CURRENT_BINARY_DIR}/${Launcher_APP_BINARY_NAME}.6.scd")
    set(out_man "${CMAKE_CURRENT_BINARY_DIR}/${Launcher_APP_BINARY_NAME}.6")
    add_custom_command(
        DEPENDS "${in_scd}"
        OUTPUT "${out_man}"
        COMMAND ${SCDOC_SCDOC} < "${in_scd}" > "${out_man}"
    )
    add_custom_target(man ALL DEPENDS ${out_man})
    set(Launcher_ManPage "program_info/${Launcher_APP_BINARY_NAME}.6" PARENT_SCOPE)
endif()
