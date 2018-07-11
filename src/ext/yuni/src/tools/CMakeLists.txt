if (NOT YUNI_AUTO_COMPILE_YUNI_CONFIG)
	if (NOT YUNI_SKIP_YUNI_CONFIG)
		add_subdirectory(yuni-config)
	endif()
endif(NOT YUNI_AUTO_COMPILE_YUNI_CONFIG)

if (YUNI_MODULE_PARSER)
	add_subdirectory(parser-generator)
endif()

if (YUNI_MODULE_DOCUMENTATION)
	#add_subdirectory(yuni-docmake)
endif()
