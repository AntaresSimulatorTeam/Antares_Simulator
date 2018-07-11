

if(ANTARES_GUI)

set(execname "antares-${ANTARES_PRG_VERSION}-ui-simulator")



include_directories("../..")
add_definitions(-DANTARES_GUI)
add_definitions(-DWX_SUPPORT)


# WxCore
include("cmake/wxcore.cmake")

# Application
include("cmake/application.cmake")
# Components
include("cmake/components.cmake")
include("cmake/components-map.cmake")
include("cmake/components-button.cmake")
include("cmake/components-htmllistbox.cmake")
# Windows
include("cmake/windows-inspector.cmake")
include("cmake/windows-options.cmake")
include("cmake/windows-calendar.cmake")
include("cmake/windows-output.cmake")
include("cmake/windows-studyparts.cmake")
include("cmake/windows-startupwizard.cmake")
# Toolsbox
include("cmake/toolbox-spotlight.cmake")
# Other
include("cmake/jobs.cmake")
include("cmake/copy-paste.cmake")
include("cmake/inputs.cmake")
include("cmake/messages.cmake")
include("cmake/resources.cmake")







set(SRCS
		# wxWidgets
		${wxWidgets_USE_FILE}
		# Config
		config.h

		# Validator
		toolbox/validator.h
		toolbox/validator.cpp

		toolbox/components/refresh.h
		toolbox/components/refresh.cpp

		# Components
		# Caption Panel
		toolbox/components/captionpanel.h
		toolbox/components/captionpanel.cpp
		# Progress bar
		toolbox/components/progressbar.h
		toolbox/components/progressbar.cpp
		# Notebook
		# Wizard header
		toolbox/components/wizardheader.h
		toolbox/components/wizardheader.cpp
		# Main panel
		toolbox/components/mainpanel.h
		toolbox/components/mainpanel.cpp

		# System
		toolbox/system/diskfreespace.hxx

		windows/simulation/panel.h
		windows/simulation/panel.cpp
		windows/simulation/run.h
		windows/simulation/run.cpp
		windows/cleaner.h
		windows/cleaner.cpp
		windows/analyzer/analyzer.cpp
		windows/analyzer/analyzer.h
		windows/saveas.h
		windows/saveas.hxx
		windows/saveas.cpp
		windows/exportmap.h
		windows/exportmap.hxx
		windows/exportmap.cpp
		windows/notes.h
		windows/notes.cpp
		windows/studylogs.h
		windows/studylogs.cpp
		windows/memorystatistics.h
		windows/memorystatistics/private.h
		windows/memorystatistics/memorystatistics.h
		windows/memorystatistics/memorystatistics.cpp
	)


if(WIN32)
	OMESSAGE("  :: generating rc/manifest files...")

	# Antares.rc
	FILE(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.o")
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.rc")
	# antares.manifest
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/manifest.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/manifest.antares-win32.manifest")


	if(MINGW)
		# resource compilation for mingw
		add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/resources/win32/antares.o"
			COMMAND windres.exe "-I${CMAKE_CURRENT_SOURCE_DIR}"
			"-i${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.rc"
			-o "${CMAKE_CURRENT_BINARY_DIR}/resources/win32/antares.o"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.rc.cmake"
			COMMENT "Generating the resource file...")
		SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/resources/win32/antares.o")

	else()
		SET(SRCS ${SRCS} "resources/win32/antares.rc")
	endif()
endif()


# For <antares/...>
include_directories("../../libs")




OMESSAGE("  :: application ${execname}")
add_executable(${execname} WIN32 ${SRCS})


set(ANTARES_LIBS
		libantares-ui-application
		libantares-ui-windows-inspector
		libantares-ui-windows-options
		libantares-ui-windows-calendar
		libantares-ui-windows-output
		libantares-ui-windows-studyparts
		libantares-ui-windows-startupwizard
		libantares-ui-jobs
		libantares-ui-resources
		libantares-ui-inputs
		libantares-ui-copy-paste
		libantares-ui-messages
		libantares-ui-wxcore
		libantares-ui-components
		libantares-ui-component-map
		libantares-ui-component-button
		libantares-ui-component-htmllistbox
		libantares-ui-component-wxscrollbox
		libantares-ui-component-panel
		libantares-ui-component-frame
		libantares-ui-component-spotlight
		libantares-ui-toolbox-spotlight
		libantares-ui-dispatcher
		libantares-ui-common
		libantares-solver-variable-info-swap
		libantares-core-swap
		libantares-core-calendar
		libantares-license
		yuni-static-core
		yuni-static-uuid
		${wxWidgets_LIBRARIES}
		${CMAKE_THREADS_LIBS_INIT}
	)

		message(STATUS "wxWidgets: wxWidgets_LIBRARIES: 	${wxWidgets_LIBRARIES}")
		message(STATUS "wxWidgets: CMAKE_THREADS_LIBS_INIT:     ${CMAKE_THREADS_LIBS_INIT}")
	
if(NOT MSVC)
	target_link_libraries(${execname}
		-Wl,--start-group ${ANTARES_LIBS} uuid dl ssh2 idn rt -Wl,--end-group)
else()
	# required for uuid
	target_link_libraries(${execname} ${ANTARES_LIBS} ole32 rpcrt4)
		
endif()

import_std_libs(${execname})
executable_strip(${execname})


# Embedding the manifest file into the executable
if(WIN32)
	if(NOT MSVC)
		# old code for mingw
		add_custom_command(TARGET antares POST_BUILD COMMAND
			copy "/B" "/Y" "\"${CMAKE_CURRENT_SOURCE_DIR}/antares.exe\""
			"\"${CMAKE_CURRENT_SOURCE_DIR}/antares-gdb.exe\""
			COMMENT "Debug mode: Preserving the original 'antares.exe' for gdb (gdb may crash with the manifest)")

		add_custom_command(TARGET antares POST_BUILD COMMAND
			"${CMAKE_CURRENT_BINARY_DIR}/../../../bin/mt.exe"
			"-nologo" "-manifest"
			"\"${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/manifest.antares-win32.manifest\""
			"-outputresource:antares.exe;1"
			COMMENT "Embedding the manifest into the executable 'antares.exe'")
	endif()
	EMBED_MANIFEST(
		"resources/win32/manifest.antares-win32.manifest"
		"${execname}.exe")
endif()


endif(ANTARES_GUI)

