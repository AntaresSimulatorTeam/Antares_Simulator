


add_library(libantares-ui-toolbox-spotlight STATIC
		toolbox/spotlight/area.cpp
		toolbox/spotlight/area.h
		toolbox/spotlight/constraint.cpp
		toolbox/spotlight/constraint.h
	)


target_link_libraries(libantares-ui-toolbox-spotlight  ${wxWidgets_LIBRARIES})

