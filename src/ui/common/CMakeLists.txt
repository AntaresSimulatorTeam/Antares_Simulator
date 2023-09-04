OMESSAGE("Antares UI Framework")


# UI Dispatcher
set(SRC_COMMON_DISPATCHER
		dispatcher.h
		dispatcher/dispatcher.h
		dispatcher/action.h
		dispatcher/action.hxx
		dispatcher/action.cpp
		dispatcher/gui.h
		dispatcher/gui.hxx
		dispatcher/gui.cpp
		dispatcher/settings.h
		dispatcher/settings.cpp
		)
source_group("dispatcher" FILES ${SRC_COMMON_DISPATCHER})
add_library(antares-ui-dispatcher ${SRC_COMMON_DISPATCHER})
target_link_libraries(antares-ui-dispatcher
		PUBLIC
			${wxWidgets_LIBRARIES}
			antares-core
		)

# Common
set(SRC_UI_COMMON
	wx-wrapper.h
	wx-wrapper.hxx
	wx-wrapper.cpp
	lock.h
	lock.cpp
	fwd.h
)
add_library(antares-ui-common ${SRC_UI_COMMON})
target_include_directories(antares-ui-common
		PUBLIC
			${CMAKE_CURRENT_SOURCE_DIR}
			${CMAKE_SOURCE_DIR}
		)

target_link_libraries(antares-ui-common
		PUBLIC
			${wxWidgets_LIBRARIES}
			antares-core
		)



# Components
set(SRC_UI_SPOTLIGHT
	component/spotlight.h
	component/spotlight/spotlight.h
	component/spotlight/spotlight.hxx
	component/spotlight/spotlight.cpp
	component/spotlight/provider.h
	component/spotlight/null-provider.h
	component/spotlight/searchtoken.h
	component/spotlight/iitem.h
	component/spotlight/iitem.hxx
	component/spotlight/iitem.cpp
	component/spotlight/separator.h
	component/spotlight/separator.cpp
	component/spotlight/text.cpp
	component/spotlight/text.h
	component/spotlight/listbox-panel.h
	component/spotlight/listbox-panel.cpp
	component/spotlight/mini-frame.cpp
	component/spotlight/mini-frame.h
	component/spotlight/search-png.hxx
)

add_library(antares-ui-component-spotlight ${SRC_UI_SPOTLIGHT})

#TODO : see if wxWidget is really PUBLIC 
target_link_libraries(antares-ui-component-spotlight
		PUBLIC
			${wxWidgets_LIBRARIES}
		PRIVATE
			antares-core
			antares-ui-common
		)

# Component panel
set(SRC_COMPONENT_PANEL
	component/panel/panel.h
	component/panel/panel.cpp
	component/panel.h
	component/panel/group.h
	component/panel/group.cpp
)
add_library(antares-ui-component-panel  ${SRC_COMPONENT_PANEL})
target_include_directories(
		antares-ui-component-panel
		PRIVATE
		${CMAKE_SOURCE_DIR}/libs
)
target_link_libraries(antares-ui-component-panel
		PUBLIC
			${wxWidgets_LIBRARIES}
		PRIVATE
			antares-core
		)

# Component frame
set(SRC_COMPONENT_FRAME
	component/frame/local-frame.h
	component/frame/local-frame.cpp
	component/frame/frame.h
	component/frame/registry.cpp
	component/frame/registry.h
)
add_library(antares-ui-component-frame  ${SRC_COMPONENT_FRAME})
target_link_libraries(antares-ui-component-frame
		PUBLIC
			${wxWidgets_LIBRARIES}
		PRIVATE
			antares-core
		)

# Scrollbox
set(SRC_SCROLLBOX
	component/scrollbox/scrollbox.h
	component/scrollbox/scrollbox.cpp
)
source_group("component\\scrollbox" FILES ${SRC_SCROLLBOX})

add_library(antares-ui-component-wxscrollbox ${SRC_SCROLLBOX})
target_link_libraries(antares-ui-component-wxscrollbox
		PUBLIC
			${wxWidgets_LIBRARIES}
		PRIVATE
			antares-core
			antares-ui-common
		)