

set(SRC_UI_WINDOWS_STARTUP_WIZARD
	windows/startupwizard.h
	windows/startupwizard.cpp
)
add_Library(antares-ui-windows-startupwizard ${SRC_UI_WINDOWS_STARTUP_WIZARD})

target_link_libraries(antares-ui-windows-startupwizard PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-windows-startupwizard PRIVATE antares-ui-common)
