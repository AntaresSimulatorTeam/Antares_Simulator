

#
# This variable must only be enabled when compiling
# the yuni library.
# Otherwise, some files could be created in the wrong place
#
if(LIBYUNI_CONFIG_ENABLED)
	if(MSVC)
		set(YUNI_LIBYUNI_CONFIG_COMPILER "msvc")
	else()
		if(MINGW)
			set(YUNI_LIBYUNI_CONFIG_COMPILER "mingw")
		else()
			set(YUNI_LIBYUNI_CONFIG_COMPILER "gcc")
		endif()
	endif()

	# Configure the config-file for the in-tree yuni-config
	set(YUNI_LIBYUNI_CONFIG_INTREE_INIFILE
		"${CMAKE_CURRENT_BINARY_DIR}/yuni.config.${YUNI_LIBYUNI_CONFIG_COMPILER}")
	file(WRITE "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "[settings]\n")

	# Configure the config-file for the installed Yuni profile
	set(YUNI_LIBYUNI_CONFIG_TARGET_INIFILE
		"${CMAKE_CURRENT_BINARY_DIR}/yuni.config.${YUNI_LIBYUNI_CONFIG_COMPILER}.target")
	file(WRITE "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "[settings]\n")
endif()


macro(LIBYUNI_CONFIG_CFLAG mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
			file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "cxxflag:${module} = ${pth}\n")
		endif()
		if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
			file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "cxxflag:${module} = ${pth}\n")
		endif()
	endif()
endmacro()


macro(LIBYUNI_CONFIG_LIB mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "lib:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "lib:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()


macro(LIBYUNI_CONFIG_LIB_RAW_COMMAND mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "lib,rawcommand:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "lib,rawcommand:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()


macro(LIBYUNI_CONFIG_FRAMEWORK mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "framework:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "framework:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()



macro(LIBYUNI_CONFIG_DEFINITION mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "define:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "define:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()


macro(LIBYUNI_CONFIG_DEPENDENCY module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "dependency:${module} = ${pth}\n")
		file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "dependency:${module} = ${pth}\n")
	endif()
endmacro()




macro(LIBYUNI_CONFIG_INCLUDE_PATH mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "path.include:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "path.include:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()


macro(LIBYUNI_CONFIG_LIB_PATH mode module pth)
	if(LIBYUNI_CONFIG_ENABLED)
		foreach(I ${pth})
			if("${mode}" STREQUAL "target" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}" "path.lib:${module} = ${I}\n")
			endif()
			if("${mode}" STREQUAL "intree" OR "${mode}" STREQUAL "both")
				file(APPEND "${YUNI_LIBYUNI_CONFIG_INTREE_INIFILE}" "path.lib:${module} = ${I}\n")
			endif()
		endforeach()
	endif()
endmacro()

