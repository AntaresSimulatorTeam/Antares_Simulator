

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
	windows/options/adequacy-patch/adequacy-patch-options.h
	windows/options/adequacy-patch/adequacy-patch-options.cpp
	windows/options/adequacy-patch/adequacy-patch-areas.h
	windows/options/adequacy-patch/adequacy-patch-areas.cpp
	windows/options/optimization/optimization.h
	windows/options/optimization/optimization.cpp
	windows/options/advanced/advanced.h
	windows/options/advanced/advanced.cpp
)
add_Library(antares-ui-windows-options ${SRC_UI_WINDOWS_OPTIONS})

target_include_directories(antares-ui-windows-options
							PRIVATE
								"${CMAKE_SOURCE_DIR}/ui/simulator"
)

target_link_libraries(antares-ui-windows-options
						PUBLIC
							${wxWidgets_LIBRARIES}
						PRIVATE
							antares-ui-common
							Antares::sys
)