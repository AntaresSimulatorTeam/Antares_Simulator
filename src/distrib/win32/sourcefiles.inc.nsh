${SetOutPath} "$INSTDIR\Sources\src"
File /nonfatal /r /x build_deps  ${ANTARES_SRC_DIR}\..\src\*.*

${SetOutPath} "$INSTDIR\Sources"
File  ${ANTARES_SRC_DIR}\..\AUTHORS.txt
File  ${ANTARES_SRC_DIR}\..\CERTIFICATE.txt
File  ${ANTARES_SRC_DIR}\..\CONTRIBUTING.md
File  ${ANTARES_SRC_DIR}\..\COPYING.txt
File  ${ANTARES_SRC_DIR}\..\NEWS.txt
File  ${ANTARES_SRC_DIR}\..\ROADMAP.txt
File  ${ANTARES_SRC_DIR}\..\THANKS.txt
File  ${ANTARES_SRC_DIR}\..\INSTALL.md
File  ${ANTARES_SRC_DIR}\..\README.md

${SetOutPath} "$INSTDIR\Sources\docs"
File /r  ${ANTARES_SRC_DIR}\..\docs\*.*

; TODO For now build_deps source are not provided

;${SetOutPath} "$INSTDIR\Sources\src\build_deps\wxWidgets\source"
;File /nonfatal /r /x .git  ${ANTARES_SRC_DIR}\..\src\build_deps\wxWidgets\source\*.*