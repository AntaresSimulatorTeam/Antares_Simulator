!addplugindir ${ANTARES_SRC_DIR}\distrib\win32

${SetOutPath} "$INSTDIR\Examples"

File ${ANTARES_SRC_DIR}\..\resources\examples.7z
Nsis7z::Extract ${ANTARES_SRC_DIR}\..\resources\examples.7z

Delete examples.7z