

set(SRC_UI_RESOURCES
		toolbox/locales.h
		toolbox/locales.cpp

		toolbox/resources.h
		toolbox/resources.cpp

)
add_Library(antares-ui-resources ${SRC_UI_RESOURCES})


target_link_libraries(antares-ui-resources PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-resources PRIVATE antares-core antares-ui-common)

