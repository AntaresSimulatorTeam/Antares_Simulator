#
# Locate the Yuni Framework
#
# This module reads the following variables :
#
#  - Yuni_YUNICONFIG_PATH   : Path to the program 'yuni-config' (optional if you're building
#                             in the source tree)
#
# This module defines :
#  - Yuni_FOUND             : If the libraries has been found
#  - Yuni_CXXFLAGS          : The flags to use for compiling a source file
#  - Yuni_LIBS              : The flags to use to link against the libyuni libraries
#  - Yuni_INFOS             : A string about the selected version of yuni (`Not found` by default)
#  - Yuni_YUNICONFIG_PATH   : Path to the program 'yuni-config' (empty if not found)
#
# Usage :
# \code
# find_package(Yuni COMPONENTS core gfx3d lua)
# if(NOT Yuni_FOUND)
#	message(ERROR "The yuni framework could not been found.")
# endif()
# \endcode
#


#
# External variables
#
set(Yuni_CXXFLAGS "")
set(Yuni_LIBS     "")
set(Yuni_INFOS    "Not found")
set(Yuni_FOUND    FALSE)
# Yuni_YUNICONFIG_PATH is also an input variable.

#
# Internal variables
#
set(__Yuni_Message "Looking for the Yuni Framework")
set(__Yuni_ConfigNotFound FALSE)

#
# Where is yuni-config ?
#
# - It might be specified.

if(Yuni_YUNICONFIG_PATH AND EXISTS "${Yuni_YUNICONFIG_PATH}")
	# Nothing to do, the binary is there, we'll blindly use it.
	set(__Yuni_Config "${Yuni_YUNICONFIG_PATH}")
elseif(Yuni_YUNICONFIG_PATH AND NOT EXISTS "${Yuni_YUNICONFIG_PATH}")
	# If the specified yuni-config cannot be found, we should not try harder.
	message(STATUS "${__Yuni_Message} - The specified yuni-config (`${Yuni_YUNICONFIG_PATH}`) could not be found.")
	set(__Yuni_ConfigNotFound TRUE)
else()
	# Try to find it in _reasonable_ places.
	set(__Yuni_CurrentFolder "${CMAKE_CURRENT_LIST_FILE}")
	string(REPLACE "\\FindYuni.cmake" "" __Yuni_CurrentFolder "${__Yuni_CurrentFolder}")
	string(REPLACE "/FindYuni.cmake" "" __Yuni_CurrentFolder "${__Yuni_CurrentFolder}")

	SET(__Yuni_ProgramSearchPath
		# Search in the source tree (if we have a trunk-like tree).
		"${__Yuni_CurrentFolder}/../../src/build/release/bin/"
		"${__Yuni_CurrentFolder}/../../src/build/debug/bin/"
		"$ENV{YUNI_PATH}")

	find_program(__Yuni_Config NAMES yuni-config yuni-config.exe PATHS ${__Yuni_ProgramSearchPath})

	if("${__Yuni_Config}" STREQUAL "__Yuni_Config-NOTFOUND")
		message(STATUS "${__Yuni_Message} - failed ('yuni-config' not found)")
		set(__Yuni_ConfigNotFound  TRUE)
	endif()
endif()


# FIXME: we should check if yuni-config has any versions configured here, to avoid problems.
# FIXME: we also should give a choice of options to pass to yuni-config (like a secondary version, or so).

if(NOT __Yuni_ConfigNotFound)

	# Store the config path.
	set(Yuni_YUNICONFIG_PATH   "${__Yuni_Config}")

	#
	# Compiler
	#
	set(__Yuni_Compiler "gcc")
	if(MINGW)
		set(__Yuni_Compiler "mingw")
	endif()
	if(MSVC)
		set(__Yuni_Compiler "msvc")
	endif()
	if(WATCOM)
		set(__Yuni_Compiler "watcom")
	endif()
	if(BORLAND)
		set(__Yuni_Compiler "borland")
	endif()
	if(MSYS)
		set(__Yuni_Compiler "mingw")
	endif(MSYS)

	#
	# Building the command line options for the list of components
	#
	set(__Yuni_ModsMods "")
	foreach(COMPONENT ${Yuni_FIND_COMPONENTS})
		string(TOLOWER ${COMPONENT} COMPONENT)
		set(__Yuni_ModsOpts "${__Yuni_ModsOpts} ${COMPONENT}")
	endforeach()

	# Converting eventual slashes in yuni-config path on Windows
    if(WIN32 OR WIN64)
        if(NOT MSYS AND NOT CYGWIN)
            # On Windows (not MSys), a path with slashes is invalid.
			# "C:/path/..." needs to be changed into "C:\path\..."
            string(REPLACE "/" "\\" __Yuni_Config "${__Yuni_Config}")
        endif()
    endif()

	# Check if the required modules, and their dependencies are compiled in.
	execute_process(COMMAND "${__Yuni_Config}" -c "${__Yuni_Compiler}"
		-m "${__Yuni_ModsOpts}" --module-deps
		OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE __Yuni_ModsDeps
		RESULT_VARIABLE __Yuni_Config_DepsResult)
	if(NOT "${__Yuni_Config_DepsResult}" EQUAL 0 OR "${__Yuni_ModsDeps}" STREQUAL "")
		message(STATUS "${__Yuni_Message} - Requires: ${Yuni_FIND_COMPONENTS}")
		message(STATUS "${__Yuni_Message} - failed - the required modules could not be found")
		set(__Yuni_ConfigNotFound TRUE)
	endif()

	# If we encountered no problems, get the framework info, and fill
	# the detection variables.
	if(NOT __Yuni_ConfigNotFound)

		# Infos
		execute_process(COMMAND "${__Yuni_Config}" -c "${__Yuni_Compiler}"
			-m "${__Yuni_ModsOpts}" -l
			OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE Yuni_INFOS)

		# CXX
		execute_process(COMMAND "${__Yuni_Config}" -c "${__Yuni_Compiler}"
			-m "${__Yuni_ModsOpts}" --cxxflags
			OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE Yuni_CXXFLAGS)

		# LIBS
		execute_process(COMMAND "${__Yuni_Config}" -c "${__Yuni_Compiler}"
			-m "${__Yuni_ModsOpts}" --libs
			OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE Yuni_LIBS)

		# Framework found
		set(Yuni_FOUND true)

	endif()

endif()

