

set(SRC_UI_RESOURCES
		toolbox/locales.h
		toolbox/locales.cpp

		toolbox/resources.h
		toolbox/resources.cpp

)
add_Library(libantares-ui-resources STATIC  ${SRC_UI_RESOURCES})


target_link_libraries(libantares-ui-resources  ${wxWidgets_LIBRARIES})

