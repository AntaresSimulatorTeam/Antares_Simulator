

SET(SRC_WINDOW_INSPECTOR
	windows/inspector.h
	windows/inspector/inspector.h
	windows/inspector/accumulator.hxx
	windows/inspector/inspector.cpp
	windows/inspector/frame.cpp
	windows/inspector/frame.h
	windows/inspector/grid.h
	windows/inspector/grid.cpp
	windows/inspector/property.update.cpp
	windows/inspector/data.h
	windows/inspector/data.hxx
	windows/inspector/data.cpp
	windows/inspector/editor-arealinks.h
	windows/inspector/editor-arealinks.cpp
	windows/inspector/editor-area-thermal-clusters.h
	windows/inspector/editor-area-thermal-clusters.cpp
	windows/inspector/editor-calendar.cpp
	windows/inspector/editor-calendar.h
)



add_library(libantares-ui-windows-inspector	${SRC_WINDOW_INSPECTOR})

target_link_libraries(libantares-ui-windows-inspector PUBLIC  ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-windows-inspector PRIVATE libantares-ui-common)

