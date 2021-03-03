
#Define executable name
set(execname "antares-${ANTARES_PRG_VERSION}-ui-simulator")

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

#define sources files
set(SRCS
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
		windows/constraints-builder/constraintsbuilder.cpp
		windows/constraints-builder/constraintsbuilder.h
		windows/saveas.h
		windows/saveas.hxx
		windows/saveas.cpp
		windows/exportmap.h
		windows/exportmap.hxx
		windows/exportmap.cpp
		windows/notes.h
		windows/notes.cpp
		windows/sets.h
		windows/sets.cpp
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
		"${CMAKE_CURRENT_BINARY_DIR}/resources/win32/antares.rc")
	# antares.manifest
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/manifest.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/resources/win32/manifest.antares-win32.manifest")

        FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/win32/antares.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/win32/")
	
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
		SET(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/resources/win32/antares.rc")
	endif()
endif()

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
		libantares-solver-constraints-builder
		libantares-solver-variable-info-swap
		libantares-core-swap #TODO : why no libantares-core ?
		libantares-core-calendar
		libantares-license
		yuni-static-core
		yuni-static-uuid
		libmodel_antares-swap #TODO : why no libmodel_antares ? link librairies added for ortools_utils.h
	)

	target_link_libraries(${execname}
			PRIVATE
				${ANTARES_LIBS}
			)


install(TARGETS ${execname} EXPORT antares-ui-simulator DESTINATION bin)

INSTALL(EXPORT antares-ui-simulator
	FILE antares-ui-simulatorConfig.cmake
	DESTINATION cmake
)

import_std_libs(${execname})
executable_strip(${execname})

copy_dependency(sirius_solver ${execname})

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
    #TODO : removed because macro doesn't work with CMake 3.x
	#EMBED_MANIFEST(
	#	"resources/win32/manifest.antares-win32.manifest"
	#	"${execname}")
endif()

#Copy resources files
FILE(COPY "${PROJECT_SOURCE_DIR}/docs/ref_guides/"  			DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/help/" FILES_MATCHING PATTERN "*.pdf")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/images/" 		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/images/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/animations/" 	DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/animations/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/" 		DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/resources/icons/")

if (WIN32)

	#Install shared .dll
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
	TYPE BIN
	FILES_MATCHING PATTERN "*.dll")

	#Install resources
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/help/" 		DESTINATION resources/help)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/images/" 		DESTINATION resources/images)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/animations/" 	DESTINATION resources/animations)
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/icons/" 		DESTINATION resources/icons)

else()

	#resources are loaded from /usr/share/antares/ANTARES_VERSION/resources
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/help/" 		DESTINATION "share/antares/${ANTARES_PRG_VERSION}/resources/help")
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/images/" 		DESTINATION "share/antares/${ANTARES_PRG_VERSION}/resources/images")
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/animations/" 	DESTINATION "share/antares/${ANTARES_PRG_VERSION}/resources/animations")
	install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/resources/icons/" 		DESTINATION "share/antares/${ANTARES_PRG_VERSION}/resources/icons")

endif()

