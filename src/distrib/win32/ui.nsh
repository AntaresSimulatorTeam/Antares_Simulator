;Memento Settings
!define MEMENTO_REGISTRY_ROOT HKLM
!define MEMENTO_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"

;Interface Settings
!define MUI_ABORTWARNING

; MUI Settings / Icons
!define MUI_ICON   "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"



!define MUI_HEADERIMAGE
; MUI Settings / Wizard
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"



!define MUI_COMPONENTSPAGE_SMALLDESC

;Pages
!define MUI_WELCOMEPAGE_TITLE "$(LngWelcome)"
!define MUI_WELCOMEPAGE_TEXT "$(LngWelcomeT)"

!insertmacro MUI_PAGE_WELCOME

; COPYING
;!insertmacro MUI_PAGE_LICENSE "COPYING-University-of-Strathclyde.txt"
;!insertmacro MUI_PAGE_LICENSE "COPYING-Elia.txt"
;!insertmacro MUI_PAGE_LICENSE "COPYING-Trial.txt"
!insertmacro MUI_PAGE_LICENSE "../../../../COPYING.txt"

!insertmacro MUI_PAGE_COMPONENTS

!define MUI_DIRECTORYPAGE_VERIFYONLEAVE
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES

; !define MUI_FINISHPAGE_LINK "Visit http://dma-tools/antares for the latest news, FAQs and support"
; !define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}"


!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\${ANTARES_BIN}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT

; !define MUI_FINISHPAGE_SHOWREADME
; !define MUI_FINISHPAGE_SHOWREADME_TEXT "Show release notes"
; !define MUI_FINISHPAGE_SHOWREADME_FUNCTION ShowReleaseNotes


!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; These indented statements modify settings for MUI_PAGE_FINISH
;  !define MUI_FINISHPAGE_NOAUTOCLOSE
!undef  MUI_INSTFILESPAGE_PROGRESSBAR
!define MUI_INSTFILESPAGE_PROGRESSBAR colored
!insertmacro MUI_PAGE_FINISH

