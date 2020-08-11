

set(SRC_UI_WINDOWS_OPTIONS
	# Options
	windows/options/temp-folder/temp-folder.h
	windows/options/temp-folder/temp-folder.cpp
	windows/options/geographic-trimming/geographic-trimming.h
	windows/options/geographic-trimming/geographic-trimming.cpp
	windows/options/select-output/select-output.h
	windows/options/select-output/select-output.cpp
	windows/options/playlist/playlist.h
	windows/options/playlist/playlist.cpp
	windows/options/optimization/optimization.h
	windows/options/optimization/optimization.cpp
	windows/options/advanced/advanced.h
	windows/options/advanced/advanced.cpp
)
add_Library(libantares-ui-windows-options ${SRC_UI_WINDOWS_OPTIONS})

target_link_libraries(libantares-ui-windows-options PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-windows-options PRIVATE libantares-ui-common)
