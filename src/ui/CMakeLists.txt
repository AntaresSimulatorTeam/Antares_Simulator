


# Location where cmake first looks for cmake modules.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})


set(ANTARES_GUI true)
#include(../cmake/common-settings.cmake)
include(../cmake/wxWidgets.cmake)

#
# Thanks to wx 2.8, which is not really a friend of gcc >=4.x ...
#
if(NOT MSVC)
#	Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif(NOT MSVC)

# Swap support
ADD_DEFINITIONS("-DANTARES_SWAP_SUPPORT=1")


if(ANTARES_GUI)
	add_subdirectory(common)
endif()

add_subdirectory(simulator)


