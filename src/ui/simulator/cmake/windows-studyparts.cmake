

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
		windows/hydro/allocation.cpp
		windows/hydro/allocation.h
		windows/thermal/cluster.h
		windows/thermal/cluster.cpp
		windows/thermal/panel.h
		windows/thermal/panel.cpp
		windows/connection.h
		windows/connection.cpp
		windows/scenario-builder/panel.h
		windows/scenario-builder/panel.cpp
		windows/bindingconstraint/bindingconstraint.h
		windows/bindingconstraint/bindingconstraint.cpp
		windows/bindingconstraint/edit.h
		windows/bindingconstraint/edit.cpp
		windows/options/scenario-builder.h
		windows/options/scenario-builder/scenario-builder.h
		windows/options/scenario-builder/scenario-builder.cpp
)
add_Library(libantares-ui-windows-studyparts STATIC  ${SRC_UI_WINDOWS_STUDYPARTS})


target_link_libraries(libantares-ui-windows-studyparts  ${wxWidgets_LIBRARIES})

