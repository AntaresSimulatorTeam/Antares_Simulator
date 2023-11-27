


add_library(antares-ui-toolbox-spotlight
		toolbox/spotlight/area.cpp
		toolbox/spotlight/area.h
		toolbox/spotlight/constraint.cpp
		toolbox/spotlight/constraint.h
	)


target_link_libraries(antares-ui-toolbox-spotlight PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-toolbox-spotlight PRIVATE antares-ui-common)

