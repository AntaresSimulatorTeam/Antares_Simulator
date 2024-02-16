

SET(SRC_WINDOW_INSPECTOR
	windows/inspector.h
	windows/inspector/inspector.h
	windows/inspector/accumulator.hxx
    windows/inspector/constants.h
    windows/inspector/constants.cpp
	windows/inspector/inspector.cpp
	windows/inspector/frame.cpp
	windows/inspector/frame.h
	windows/inspector/grid.h
	windows/inspector/grid.cpp
	windows/inspector/property.update.cpp
	windows/inspector/property.cluster.update.cpp
	windows/inspector/property.cluster.update.h
	windows/inspector/data.h
	windows/inspector/data.hxx
	windows/inspector/data.cpp
	windows/inspector/editor-calendar.cpp
	windows/inspector/editor-calendar.h
)



add_library(antares-ui-windows-inspector	${SRC_WINDOW_INSPECTOR})

target_link_libraries(antares-ui-windows-inspector
		PUBLIC
		${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-windows-inspector
		PRIVATE
		antares-ui-common
		Antares::resources
		Antares::locator
		Antares::study
)

