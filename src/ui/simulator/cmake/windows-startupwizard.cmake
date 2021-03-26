

set(SRC_UI_WINDOWS_STARTUP_WIZARD
	windows/startupwizard.h
	windows/startupwizard.cpp
)
add_Library(libantares-ui-windows-startupwizard ${SRC_UI_WINDOWS_STARTUP_WIZARD})

target_link_libraries(libantares-ui-windows-startupwizard PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-windows-startupwizard PRIVATE libantares-license)
target_link_libraries(libantares-ui-windows-startupwizard PRIVATE libantares-ui-common)
