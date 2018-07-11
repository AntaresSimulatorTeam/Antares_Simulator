

SET(SRC_TOOLBOX_COM_BUTTON
		# Filters
		toolbox/components/button.h
		toolbox/components/button/button.h
		toolbox/components/button/button.hxx
		toolbox/components/button/button.cpp
		)


add_library(libantares-ui-component-button STATIC
	# Button
	${SRC_TOOLBOX_COM_BUTTON}
	)


target_link_libraries(libantares-ui-component-button  ${wxWidgets_LIBRARIES})
