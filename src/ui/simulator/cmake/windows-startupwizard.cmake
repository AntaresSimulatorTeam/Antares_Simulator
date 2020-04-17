

set(SRC_UI_WINDOWS_STARTUP_WIZARD
	windows/startupwizard.h
	windows/startupwizard.cpp
	windows/proxy/proxysetup.cpp
	windows/proxy/proxysetup.h
)
add_Library(libantares-ui-windows-startupwizard STATIC  ${SRC_UI_WINDOWS_STARTUP_WIZARD})

target_link_libraries(libantares-ui-windows-startupwizard  ${wxWidgets_LIBRARIES} libantares-license)
