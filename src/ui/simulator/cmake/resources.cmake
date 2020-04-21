

set(SRC_UI_RESOURCES
		toolbox/locales.h
		toolbox/locales.cpp

		toolbox/resources.h
		toolbox/resources.cpp

)
add_Library(libantares-ui-resources ${SRC_UI_RESOURCES})


target_link_libraries(libantares-ui-resources PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-resources PRIVATE libantares-core libantares-ui-common)

