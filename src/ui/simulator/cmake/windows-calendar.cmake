
set(SRC_UI_WINDOWS_CALENDAR
	# Options
	windows/calendar/calendar.cpp
	windows/calendar/calendar.h
	windows/calendar/view-standard.hxx
)
add_Library(libantares-ui-windows-calendar ${SRC_UI_WINDOWS_CALENDAR})

target_link_libraries(libantares-ui-windows-calendar PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-windows-calendar PRIVATE libantares-ui-common)
