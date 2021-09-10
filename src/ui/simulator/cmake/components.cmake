

SET(SRC_TOOLBOX_COM_WIPPANEL
		toolbox/components/wip-panel.h
		toolbox/components/wip-panel.cpp
		)
source_group("Button" FILES ${SRC_TOOLBOX_COM_WIPPANEL})


SET(SRC_TOOLBOX_COM_DBGRID
		# Filters
		toolbox/components/datagrid/filter/filter.h
		toolbox/components/datagrid/filter/filter.cpp
		toolbox/components/datagrid/filter/input.h
		toolbox/components/datagrid/filter/input.cpp
		toolbox/components/datagrid/filter/static.cpp
		toolbox/components/datagrid/filter/all/any.h
		toolbox/components/datagrid/filter/all/houryear.h
		toolbox/components/datagrid/filter/all/hour.h
		toolbox/components/datagrid/filter/all/weekday.h
		toolbox/components/datagrid/filter/all/week.h
		toolbox/components/datagrid/filter/all/week.cpp
		toolbox/components/datagrid/filter/all/month.h
		toolbox/components/datagrid/filter/all/month.cpp
		toolbox/components/datagrid/filter/all/dayyear.h
		toolbox/components/datagrid/filter/all/day.h
		toolbox/components/datagrid/filter/all/columnindex.h
		# Filters - Component
		toolbox/components/datagrid/filter/component.h
		toolbox/components/datagrid/filter/component.cpp

		# Datagrid - Component
		toolbox/components/datagrid/gridhelper.h
		toolbox/components/datagrid/gridhelper.hxx
		toolbox/components/datagrid/gridhelper.cpp
		toolbox/components/datagrid/component.h
		toolbox/components/datagrid/component.cpp
		toolbox/components/datagrid/modifiers.hxx
		toolbox/components/datagrid/wxgrid-renderer.h
		toolbox/components/datagrid/wxgrid-renderer.cpp
		toolbox/components/datagrid/dbgrid.h
		toolbox/components/datagrid/dbgrid.cpp
		toolbox/components/datagrid/default.h
		)
source_group("Datagrid\\Src" FILES ${SRC_TOOLBOX_COM_DBGRID})

SET(SRC_TOOLBOX_COM_DBGRID_RENDERERS
		toolbox/components/datagrid/renderer.h
		toolbox/components/datagrid/renderer.hxx
		toolbox/components/datagrid/renderer.cpp
		toolbox/components/datagrid/selectionoperation.h
		toolbox/components/datagrid/renderer/area.h
		toolbox/components/datagrid/renderer/area.cpp
		toolbox/components/datagrid/renderer/analyzer/areas.h
		toolbox/components/datagrid/renderer/analyzer/areas.cpp
		toolbox/components/datagrid/renderer/constraintsbuilder/links.h
		toolbox/components/datagrid/renderer/constraintsbuilder/links.cpp
		toolbox/components/datagrid/renderer/connection.h
		toolbox/components/datagrid/renderer/connection.cpp
		toolbox/components/datagrid/renderer/cell.h
		toolbox/components/datagrid/renderer/cell.cpp
		toolbox/components/datagrid/renderer/column.h
		toolbox/components/datagrid/renderer/column.cpp
		toolbox/components/datagrid/renderer/ts-management.h
		toolbox/components/datagrid/renderer/ts-management.cpp
		toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.h
		toolbox/components/datagrid/renderer/ts-management-clusters-as-renewables.cpp
		toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.h
		toolbox/components/datagrid/renderer/ts-management-aggregated-as-renewables.cpp
		toolbox/components/datagrid/renderer/geographic-trimming-grid.h
		toolbox/components/datagrid/renderer/geographic-trimming-grid.cpp
		toolbox/components/datagrid/renderer/select-variables.h
		toolbox/components/datagrid/renderer/select-variables.cpp
		toolbox/components/datagrid/renderer/mc-playlist.cpp
		toolbox/components/datagrid/renderer/mc-playlist.h
		toolbox/components/datagrid/renderer/logfile.cpp
		toolbox/components/datagrid/renderer/logfile.h
		toolbox/components/datagrid/renderer/scenario-builder-renderer-base.cpp
		toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h
		toolbox/components/datagrid/renderer/scenario-builder-load-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-load-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-thermal-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-thermal-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-renewable-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-renewable-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-hydro-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-hydro-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-wind-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-wind-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-solar-renderer.cpp
		toolbox/components/datagrid/renderer/scenario-builder-solar-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-hydro-levels-renderer.h
		toolbox/components/datagrid/renderer/scenario-builder-hydro-levels-renderer.cpp
		toolbox/components/datagrid/renderer/layers.cpp
		toolbox/components/datagrid/renderer/layers.h
		)
source_group("Datagrid\\Renderer" FILES ${SRC_TOOLBOX_COM_DBGRID_RENDERERS})

SET(SRC_SCENARIO_BUILDER_UTILS ${CMAKE_SOURCE_DIR}/libs/antares/study/scenario-builder/scBuilderUtils.cpp)

SET(SRC_TOOLBOX_COM_DBGRID_RENDERERS_MATRIX
		# Matrix
		toolbox/components/datagrid/renderer/matrix.h
		toolbox/components/datagrid/renderer/matrix.hxx
		)
source_group("Datagrid\\Renderer\\Matrix" FILES ${SRC_TOOLBOX_COM_DBGRID_RENDERERS_MATRIX})


SET(SRC_TOOLBOX_COM_DBGRID_RENDERERS_AREAS
		# Areas
		toolbox/components/datagrid/renderer/area/reservoirlevels.h
		toolbox/components/datagrid/renderer/area/reservoirlevels.cpp
		toolbox/components/datagrid/renderer/area/watervalues.h
		toolbox/components/datagrid/renderer/area/watervalues.cpp
		toolbox/components/datagrid/renderer/area/inflowpattern.h
		toolbox/components/datagrid/renderer/area/inflowpattern.cpp
		toolbox/components/datagrid/renderer/correlation.h
		toolbox/components/datagrid/renderer/correlation.cpp
		toolbox/components/datagrid/renderer/area/timeseries.h
		toolbox/components/datagrid/renderer/area/timeseries.cpp
		toolbox/components/datagrid/renderer/area/hydroprepro.h
		toolbox/components/datagrid/renderer/area/hydroprepro.cpp
		toolbox/components/datagrid/renderer/area/hydromonthlypower.h
		toolbox/components/datagrid/renderer/area/hydromonthlypower.cpp
		toolbox/components/datagrid/renderer/area/creditmodulations.h
		toolbox/components/datagrid/renderer/area/creditmodulations.cpp
		toolbox/components/datagrid/renderer/area/thermalprepro.h
		toolbox/components/datagrid/renderer/area/thermalprepro.cpp
		toolbox/components/datagrid/renderer/area/thermalmodulation.h
		toolbox/components/datagrid/renderer/area/thermalmodulation.cpp
		toolbox/components/datagrid/renderer/area/dsm.h
		toolbox/components/datagrid/renderer/area/dsm.cpp
		toolbox/components/datagrid/renderer/area/misc.h
		toolbox/components/datagrid/renderer/area/misc.cpp
		toolbox/components/datagrid/renderer/area/common.areasummary.cpp
		toolbox/components/datagrid/renderer/area/common.areasummary.h
		toolbox/components/datagrid/renderer/area/thermal.areasummary.cpp
		toolbox/components/datagrid/renderer/area/thermal.areasummary.h
		toolbox/components/datagrid/renderer/area/renewable.areasummary.cpp
		toolbox/components/datagrid/renderer/area/renewable.areasummary.h
		toolbox/components/datagrid/renderer/area/xcast-k.h
		toolbox/components/datagrid/renderer/area/xcast-k.hxx
		toolbox/components/datagrid/renderer/area/xcast-coefficients.h
		toolbox/components/datagrid/renderer/area/xcast-coefficients.hxx
		toolbox/components/datagrid/renderer/area/xcast-translation.h
		toolbox/components/datagrid/renderer/area/xcast-translation.hxx
		toolbox/components/datagrid/renderer/area/xcast-conversion.h
		toolbox/components/datagrid/renderer/area/xcast-conversion.hxx
		toolbox/components/datagrid/renderer/area/xcast-allareas.h
		toolbox/components/datagrid/renderer/area/xcast-allareas.hxx
		toolbox/components/datagrid/renderer/area/nodal-optimization.h
		toolbox/components/datagrid/renderer/area/nodal-optimization.cpp

		toolbox/components/datagrid/renderer/area/hydro/allocation.h
		toolbox/components/datagrid/renderer/area/hydro/allocation.cpp
		)
source_group("Datagrid\\Renderer\\Areas" FILES ${SRC_TOOLBOX_COM_DBGRID_RENDERERS_AREAS})

SET(SRC_TOOLBOX_COM_DBGRID_RENDERERS_LINKS
		# Links
		toolbox/components/datagrid/renderer/links/summary.h
		toolbox/components/datagrid/renderer/links/summary.cpp
		)
source_group("Datagrid\\Renderer\\Links" FILES ${SRC_TOOLBOX_COM_DBGRID_RENDERERS_LINKS})


SET(SRC_TOOLBOX_COM_DBGRID_RENDERERS_BINDING_CONSTRAINTS
		# Binding Constraints
		toolbox/components/datagrid/renderer/bindingconstraint/weights.h
		toolbox/components/datagrid/renderer/bindingconstraint/weights.cpp
		toolbox/components/datagrid/renderer/bindingconstraint/offsets.h
		toolbox/components/datagrid/renderer/bindingconstraint/offsets.cpp
		toolbox/components/datagrid/renderer/bindingconstraint/data.h
		toolbox/components/datagrid/renderer/bindingconstraint/data.cpp
		)
source_group("Datagrid\\Renderer\\Binding Constraints" FILES ${SRC_TOOLBOX_COM_DBGRID_RENDERERS_BINDING_CONSTRAINTS})



SET(SRC_TOOLBOX_COM_DBGRID_FILTER_OPERATORS
		# Filters - Operators
		toolbox/components/datagrid/filter/operator.h
		toolbox/components/datagrid/filter/operator.cpp
		toolbox/components/datagrid/filter/operator.list.h
		toolbox/components/datagrid/filter/operator.list.cpp
		toolbox/components/datagrid/filter/operator/lessthan.h
		toolbox/components/datagrid/filter/operator/lessthanorequalsto.h
		toolbox/components/datagrid/filter/operator/greaterthan.h
		toolbox/components/datagrid/filter/operator/greaterthanorequalsto.h
		toolbox/components/datagrid/filter/operator/equals.h
		toolbox/components/datagrid/filter/operator/notequals.h
		toolbox/components/datagrid/filter/operator/all.h
		toolbox/components/datagrid/filter/parameter/parameter.h
		toolbox/components/datagrid/filter/parameter/parameter.cpp
		)
source_group("Datagrid\\Filter\\Operator" FILES ${SRC_TOOLBOX_COM_DBGRID_FILTER_OPERATORS})





SET(SRC_TOOLBOX_COM_HTMLLISTBOX
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

		toolbox/components/htmllistbox/item/cluster-item.h
		toolbox/components/htmllistbox/item/cluster-item.cpp
		toolbox/components/htmllistbox/item/thermal-cluster-item.h
		toolbox/components/htmllistbox/item/thermal-cluster-item.cpp
		toolbox/components/htmllistbox/item/renewable-cluster-item.h
		toolbox/components/htmllistbox/item/renewable-cluster-item.cpp
		toolbox/components/htmllistbox/item/bindingconstraint.h
		toolbox/components/htmllistbox/item/bindingconstraint.cpp
		toolbox/components/htmllistbox/item/bindingconstraint.hxx
		toolbox/components/htmllistbox/item/area.hxx
		toolbox/components/htmllistbox/item/pathlistitem.h
		toolbox/components/htmllistbox/item/pathlistitem.cpp
		toolbox/components/htmllistbox/datasource/datasource.h
		toolbox/components/htmllistbox/datasource/datasource.hxx
		toolbox/components/htmllistbox/datasource/areas.h
		toolbox/components/htmllistbox/datasource/areas.cpp
		toolbox/components/htmllistbox/datasource/cluster-order.h
		toolbox/components/htmllistbox/datasource/cluster-order.cpp
		toolbox/components/htmllistbox/datasource/thermal-cluster-order.h
		toolbox/components/htmllistbox/datasource/thermal-cluster-order.cpp
		toolbox/components/htmllistbox/datasource/renewable-cluster-order.h
		toolbox/components/htmllistbox/datasource/renewable-cluster-order.cpp
		toolbox/components/htmllistbox/datasource/bindingconstraint.h
		toolbox/components/htmllistbox/datasource/bindingconstraint.cpp
)
source_group("HHTMLListBox" FILES ${SRC_TOOLBOX_COM_HTMLLISTBOX})




SET(SRC_TOOLBOX_COM_NOTEBOOK
	toolbox/components/notebook/notebook.h
	toolbox/components/notebook/mapnotebook.h
	toolbox/components/notebook/notebook.hxx
	toolbox/components/notebook/notebook.cpp
	toolbox/components/notebook/mapnotebook.cpp
)
source_group("Notebook" FILES ${SRC_TOOLBOX_COM_NOTEBOOK})



add_library(libantares-ui-components
	# WIP Panel (Work In progress)
	${SRC_TOOLBOX_COM_WIPPANEL}
	# Datagrid
	${SRC_TOOLBOX_COM_DBGRID_RENDERERS}
	${SRC_SCENARIO_BUILDER_UTILS}
	${SRC_TOOLBOX_COM_DBGRID_RENDERERS_MATRIX}
	${SRC_TOOLBOX_COM_DBGRID_RENDERERS_AREAS}
	${SRC_TOOLBOX_COM_DBGRID_RENDERERS_LINKS}
	${SRC_TOOLBOX_COM_DBGRID_RENDERERS_BINDING_CONSTRAINTS}
	${SRC_TOOLBOX_COM_DBGRID_FILTER_OPERATORS}
	${SRC_TOOLBOX_COM_DBGRID}
	# HTMLLISTBOX
	${SRC_TOOLBOX_COM_HTMLLISTBOX}
	# Notebook
	${SRC_TOOLBOX_COM_NOTEBOOK}
	)

target_include_directories(libantares-ui-components
							PRIVATE
								"${CMAKE_SOURCE_DIR}/ui/simulator"
)

target_link_libraries(libantares-ui-components
						PUBLIC
							${wxWidgets_LIBRARIES}
						PRIVATE
							libantares-core
							libantares-ui-common
)

