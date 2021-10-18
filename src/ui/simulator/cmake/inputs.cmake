

set(SRC_UI_INPUTS
		toolbox/input/input.h
		toolbox/input/input.cpp
		toolbox/input/area.h
		toolbox/input/area.cpp
		toolbox/input/connection.h
		toolbox/input/connection.cpp
		toolbox/input/thermal-cluster.h
		toolbox/input/thermal-cluster.cpp
		toolbox/input/renewable-cluster.h
		toolbox/input/renewable-cluster.cpp
		toolbox/input/bindingconstraint.h
		toolbox/input/bindingconstraint/bindingconstraint.h
		toolbox/input/bindingconstraint/bindingconstraint.cpp
)
add_Library(libantares-ui-inputs  ${SRC_UI_INPUTS})


target_link_libraries(libantares-ui-inputs PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-inputs PRIVATE libantares-core libantares-ui-common)
