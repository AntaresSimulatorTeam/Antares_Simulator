

if (USE_PRECOMPILED_EXT AND UNIX)
    set (wxWidgets_CONFIG_EXECUTABLE ${DEPS_INSTALL_DIR}/bin/wx-config)
endif()

#wxWidget required
find_package(wxWidgets REQUIRED COMPONENTS net core base richtext propgrid aui adv html xml)
include(${wxWidgets_USE_FILE})

# TODO : remove ADD_DEFINITIONS Swap support
ADD_DEFINITIONS("-DANTARES_SWAP_SUPPORT=1")

# Add link library for static use of wxWidgets
if("${VCPKG_TARGET_TRIPLET}" STREQUAL "x64-windows-static" OR "${VCPKG_TARGET_TRIPLET}" STREQUAL "x86-windows-static")

	#Need to also manualy add find package
	find_package(jpeg REQUIRED)
	find_package(tiff REQUIRED)
	find_package(png REQUIRED)
	find_package(expat REQUIRED)

    SET(wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${TIFF_LIBRARIES} ${EXPAT_LIBRARIES})
	
endif()

add_subdirectory(common)	#All UI libraries
add_subdirectory(simulator)	#Simulator application

