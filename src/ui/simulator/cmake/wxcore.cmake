

set(SRCTOOLBOX_CORE_WX
		# Resources
		toolbox/resources.h
		toolbox/resources.cpp
		toolbox/create.h
		toolbox/create.hxx
		toolbox/create.cpp
)
add_Library(libantares-ui-wxcore STATIC  ${SRCTOOLBOX_CORE_WX})


