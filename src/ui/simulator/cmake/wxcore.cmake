set(SRCTOOLBOX_CORE_WX
		# Resources
		toolbox/resources.h
		toolbox/resources.cpp
		toolbox/create.h
		toolbox/create.hxx
		toolbox/create.cpp
)
add_Library(libantares-ui-wxcore ${SRCTOOLBOX_CORE_WX})
target_link_libraries(libantares-ui-wxcore PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-wxcore PRIVATE libantares-ui-common)


