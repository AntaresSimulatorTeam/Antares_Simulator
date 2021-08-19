

set(SRC_UI_WINDOWS_STUDYPARTS
		windows/correlation/correlation.h
		windows/correlation/datasources.hxx
		windows/correlation/correlation.cpp
		windows/xcast/xcast.h
		windows/xcast/xcast.hxx
		windows/hydro/series.h
		windows/hydro/series.cpp
		windows/hydro/prepro.h
		windows/hydro/prepro.cpp
		windows/hydro/dailypower.h
		windows/hydro/dailypower.cpp
		windows/hydro/allocation.cpp
		windows/hydro/allocation.h
		windows/hydro/localdatahydro.h
		windows/hydro/localdatahydro.cpp
		windows/hydro/management.h
		windows/hydro/management.cpp
		windows/hydro/levelsandvalues.h
		windows/hydro/levelsandvalues.cpp
		windows/thermal/cluster.h
		windows/thermal/cluster.cpp
		windows/renewables/cluster.h
		windows/renewables/cluster.cpp
		windows/thermal/panel.h
		windows/thermal/panel.cpp
		windows/renewables/panel.h
		windows/renewables/panel.cpp
		windows/connection.h
		windows/connection.cpp
		windows/scenario-builder/panel.h
		windows/scenario-builder/panel.cpp
		windows/bindingconstraint/bindingconstraint.h
		windows/bindingconstraint/bindingconstraint.cpp
		windows/bindingconstraint/edit.h
		windows/bindingconstraint/edit.cpp
)
add_Library(libantares-ui-windows-studyparts  ${SRC_UI_WINDOWS_STUDYPARTS})


target_link_libraries(libantares-ui-windows-studyparts PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-windows-studyparts PRIVATE libantares-ui-common)

