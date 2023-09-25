

SET(SRC_TOOLBOX_COM_BUTTON
		# Filters
		toolbox/components/button.h
		toolbox/components/button/button.h
		toolbox/components/button/button.hxx
		toolbox/components/button/button.cpp
		)


add_library(antares-ui-component-button
	# Button
	${SRC_TOOLBOX_COM_BUTTON}
	)


target_link_libraries(antares-ui-component-button PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-component-button PRIVATE antares-core antares-ui-common)
