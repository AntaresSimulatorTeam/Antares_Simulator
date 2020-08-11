

SET(SRC_TOOLBOX_COM_BUTTON
		# Filters
		toolbox/components/button.h
		toolbox/components/button/button.h
		toolbox/components/button/button.hxx
		toolbox/components/button/button.cpp
		)


add_library(libantares-ui-component-button
	# Button
	${SRC_TOOLBOX_COM_BUTTON}
	)


target_link_libraries(libantares-ui-component-button PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-component-button PRIVATE libantares-core libantares-ui-common)
