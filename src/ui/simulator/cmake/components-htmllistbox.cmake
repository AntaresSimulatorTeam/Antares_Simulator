add_library(libantares-ui-component-htmllistbox
		# HTMLListbox
		toolbox/components/htmllistbox/component.h
		toolbox/components/htmllistbox/component.hxx
		toolbox/components/htmllistbox/component.cpp
		toolbox/components/htmllistbox/item/item.h
		toolbox/components/htmllistbox/item/item.hxx
		toolbox/components/htmllistbox/item/item.cpp
		toolbox/components/htmllistbox/sort.h
		toolbox/components/htmllistbox/item/info.h toolbox/components/htmllistbox/item/info.cpp
		toolbox/components/htmllistbox/item/group.h toolbox/components/htmllistbox/item/group.cpp
		toolbox/components/htmllistbox/item/area.h
		toolbox/components/htmllistbox/item/area.cpp
		toolbox/components/htmllistbox/item/area.hxx

		toolbox/components/htmllistbox/item/error.h
		toolbox/components/htmllistbox/item/error.cpp

		toolbox/components/htmllistbox/item/thermal-cluster-item.h
		toolbox/components/htmllistbox/item/thermal-cluster-item.cpp
		toolbox/components/htmllistbox/item/renewable-cluster-item.h
		toolbox/components/htmllistbox/item/renewable-cluster-item.cpp
		toolbox/components/htmllistbox/item/area.hxx
		toolbox/components/htmllistbox/item/pathlistitem.h
		toolbox/components/htmllistbox/item/pathlistitem.cpp
		toolbox/components/htmllistbox/datasource/datasource.h
		toolbox/components/htmllistbox/datasource/datasource.hxx

        toolbox/components/htmllistbox/datasource/cluster-order.h
        toolbox/components/htmllistbox/datasource/cluster-order.cpp
		toolbox/components/htmllistbox/datasource/thermal-cluster-order.h
		toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp
		toolbox/components/htmllistbox/datasource/renewable-cluster-order.h
		toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp

        toolbox/components/htmllistbox/item/cluster-item.h
        toolbox/components/htmllistbox/item/cluster-item.cpp        
        toolbox/components/htmllistbox/item/renewable-cluster-item.h
        toolbox/components/htmllistbox/item/renewable-cluster-item.cpp
        toolbox/components/htmllistbox/item/thermal-cluster-item.h
        toolbox/components/htmllistbox/item/thermal-cluster-item.cpp
	)


target_link_libraries(libantares-ui-component-htmllistbox  PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-component-htmllistbox PRIVATE libantares-core libantares-ui-common)
