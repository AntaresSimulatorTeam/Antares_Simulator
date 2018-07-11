project(AntaresUICommon)
# define minimum cmake version
cmake_minimum_required(VERSION 2.8)


include("../../cmake/messages.cmake")
OMESSAGE("Antares UI Framework")



include_directories("../../")
link_libraries(${wxWidgets_LIBRARIES})


# UI Dispatcher
include(cmake/dispatcher.cmake)
# Components
include(cmake/component-spotlight.cmake)


set(SRC_UI_COMMON
	wx-wrapper.h
	wx-wrapper.hxx
	wx-wrapper.cpp
	lock.h
	lock.cpp
	fwd.h
)
add_library(libantares-ui-common  STATIC ${SRC_UI_COMMON})



set(SRC_COMPONENT_PANEL
	component/panel/panel.h
	component/panel/panel.cpp
	component/panel.h
	component/panel/group.h
	component/panel/group.cpp
)
add_library(libantares-ui-component-panel  STATIC ${SRC_COMPONENT_PANEL})

set(SRC_COMPONENT_FRAME
	component/frame/local-frame.h
	component/frame/local-frame.cpp
	component/frame/frame.h
	component/frame/registry.cpp
	component/frame/registry.h
)
add_library(libantares-ui-component-frame  STATIC ${SRC_COMPONENT_FRAME})

set(SRC_SCROLLBOX
	component/scrollbox/scrollbox.h
	component/scrollbox/scrollbox.cpp
)
source_group("component\\scrollbox" FILES ${SRC_SCROLLBOX})

add_library(libantares-ui-component-wxscrollbox STATIC
	# Scrollbox
	${SRC_SCROLLBOX}
)

