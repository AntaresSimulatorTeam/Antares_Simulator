
if(ANTARES_GUI)

	if(WIN32)
		set(ANTARES_WX_VERSION_HI		3)
		set(ANTARES_WX_VERSION_LO		0)

		set(ANTARES_WX_VERSION			30)

		set(ANTARES_WX_ROOT_PATH "${PROJECT_SOURCE_DIR}/ext/wxwidgets")
		
		
			
	
		OMESSAGE("wxWidgets version to set: ${ANTARES_WX_VERSION_HI}.${ANTARES_WX_VERSION_LO}")							
		OMESSAGE("${ANTARES_WX_ROOT_PATH}")

		

	endif()

	include("../FindWXWidgets.cmake")
	
endif()