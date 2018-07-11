
YMESSAGE_MODULE("Device::Display")

LIBYUNI_CONFIG_DEPENDENCY("display" "devices") # devices is required


if(UNIX AND NOT APPLE AND NOT HAIKU AND NOT BEOS)
	find_package(X11)
	if (NOT X11_FOUND)
		YMESSAGE(    "[!!] Impossible to find X11 headers")
		YMESSAGE(    " * Packages needed on debian: libx11-dev")
		YMESSAGE(    " * Packages needed on redhat: xorg-x11-devel")
	endif()
	#if (NOT X11_Xext_FOUND)
#		YERROR(    "Impossible to find X11 Xext extension headers")
	#	YMESSAGE(    " * Packages needed on debian: x11proto-xext-dev")
#		YMESSAGE(    " * Packages needed on redhat: xorg-x11-devel")
#	endif()
	check_include_file_cxx("X11/extensions/randr.h" YUNI_HAS_X11_EXTENSION_RANDR)
	if (NOT YUNI_HAS_X11_EXTENSION_RANDR)
		YERROR(    "Impossible to find X11 Xrandr extension headers")
		YMESSAGE(    " * Packages needed on debian: x11proto-randr-dev")
		YMESSAGE(    " * Packages needed on redhat: libxcb-randr")
	endif()

	LIBYUNI_CONFIG_LIB("both" "display" "X11")
	LIBYUNI_CONFIG_LIB("both" "display" "Xext")
	LIBYUNI_CONFIG_LIB("both" "display" "Xrandr")
	LIBYUNI_CONFIG_LIB_PATH("both" "display" "${X11_LIBRARY_DIR}")
endif(UNIX AND NOT APPLE AND NOT HAIKU AND NOT BEOS)

if(UNIX)
	if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
		include_directories("/usr/local/include")
		LIBYUNI_CONFIG_LIB_PATH("both" "display" "/usr/local/include")
	endif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
endif(UNIX)


if(APPLE)
	LIBYUNI_CONFIG_FRAMEWORK("both" "display" CoreFoundation)
	LIBYUNI_CONFIG_FRAMEWORK("both" "display" Cocoa)
	LIBYUNI_CONFIG_FRAMEWORK("both" "display" IOKit)
endif(APPLE)





# Devices
set(SRC_DEVICE_DISPLAY
		device/display/list/list.h
		device/display/list/list.cpp
		device/display/list/list.hxx
		device/display/list.h
		device/display/list/windows.hxx
		device/display/list/macosx.hxx
		device/display/list/linux.hxx
		device/display/monitor.h
		device/display/monitor.hxx
		device/display/monitor.cpp
		device/display/resolution.h
		device/display/resolution.hxx
		device/display/resolution.cpp)
source_group(Devices\\Display FILES ${SRC_DEVICE_DISPLAY})


add_library(yuni-static-device-display STATIC
			yuni.h
			${SRC_DEVICE_DISPLAY}
)

# Setting output path
set_target_properties(yuni-static-device-display PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-device-display
		COMPONENT ${YUNICOMPONENT_DEVICE_DISPLAY}
		ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install Device-related headers
install(
	DIRECTORY device
	COMPONENT ${YUNICOMPONENT_DEVICE_DISPLAY}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)

target_link_libraries(yuni-static-device-display yuni-static-core)


