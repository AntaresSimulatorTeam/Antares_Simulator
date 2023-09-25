
set(SRC_UI_WINDOWS_CALENDAR
	# Options
	windows/calendar/calendar.cpp
	windows/calendar/calendar.h
	windows/calendar/view-standard.hxx
)
add_Library(antares-ui-windows-calendar ${SRC_UI_WINDOWS_CALENDAR})

target_link_libraries(antares-ui-windows-calendar PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(antares-ui-windows-calendar PRIVATE antares-ui-common)
