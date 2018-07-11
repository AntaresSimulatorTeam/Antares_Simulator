

SET(SRC_TOOLBOX_COM_MAP
		toolbox/components/map/settings.h
		toolbox/components/map/settings.cpp
		toolbox/components/map/controls/addtools.h
		toolbox/components/map/controls/addtools.cpp
		toolbox/components/map/drawingcontext.h
		toolbox/components/map/drawingcontext.cpp
		toolbox/components/map/manager.h
		toolbox/components/map/manager.hxx
		toolbox/components/map/manager.cpp
		toolbox/components/map/control.h
		toolbox/components/map/control.cpp
		toolbox/components/map/infos.h
		toolbox/components/map/infos.cpp
		toolbox/components/map/component.h
		toolbox/components/map/component.cpp
		)
source_group("Src" FILES ${SRC_TOOLBOX_COM_MAP})

SET(SRC_TOOLBOX_COM_MAP_TOOLS
		toolbox/components/map/tools/tool.h
		toolbox/components/map/tools/tool.cpp
		toolbox/components/map/tools/connectioncreator.h
		toolbox/components/map/tools/connectioncreator.cpp
		toolbox/components/map/tools/remover.h
		toolbox/components/map/tools/remover.cpp
		)
source_group("Tools" FILES ${SRC_TOOLBOX_COM_MAP_TOOLS})

SET(SRC_TOOLBOX_COM_MAP_NODES
		toolbox/components/map/nodes/item.h
		toolbox/components/map/nodes/item.cpp
		toolbox/components/map/nodes/connection.h
		toolbox/components/map/nodes/connection.hxx
		toolbox/components/map/nodes/connection.cpp
		toolbox/components/map/nodes/node.h
		toolbox/components/map/nodes/node.cpp
		toolbox/components/map/nodes/bindingconstraint.h
		toolbox/components/map/nodes/bindingconstraint.cpp
		)
source_group("Nodes" FILES ${SRC_TOOLBOX_COM_MAP_NODES})




add_library(libantares-ui-component-map STATIC
	# Map
	${SRC_TOOLBOX_COM_MAP_TOOLS}
	${SRC_TOOLBOX_COM_MAP_NODES}
	${SRC_TOOLBOX_COM_MAP}
	)


target_link_libraries(libantares-ui-component-map  ${wxWidgets_LIBRARIES})
