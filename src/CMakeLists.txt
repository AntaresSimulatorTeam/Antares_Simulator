project(antares)
cmake_minimum_required(VERSION 2.8)

# Version
set(ANTARES_VERSION_HI	      7)
set(ANTARES_VERSION_LO	      0)
set(ANTARES_VERSION_REVISION  0)
set(ANTARES_VERSION_YEAR      2018)

set(ANTARES_PUBLISHER	   "RTE France")
set(ANTARES_WEBSITE	   	   "https://antares.rte-france.com")

# Beta release
set(ANTARES_BETA 0)
#set(ANTARES_RC 1)















#
# ----------------------------------------------------------------------------------------------
#


# CMAKE - Suppress useless features
set(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES true)
set(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES true)

include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)

include("cmake/messages.cmake")

if(${ANTARES_BETA})
	set(ANTARES_VERSION_TAG "-beta${ANTARES_BETA}")
else()
	set(ANTARES_VERSION_TAG "")
endif()

if(${ANTARES_RC})
	set (ANTARES_VERSION_TAG "${ANTARES_VERSION_TAG}-rc${ANTARES_RC}")
else()
	set (ANTARES_RC 0)
endif()

# Build Configuration
if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
	set(ANTARES_TARGET "${CMAKE_BUILD_TYPE}")
else()
	set(CMAKE_BUILD_TYPE "debug")
	set(ANTARES_TARGET  "debug")
endif()

OMESSAGE("")
OMESSAGE_TITLE("Antares " "v${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}.${ANTARES_VERSION_REVISION}${ANTARES_VERSION_TAG}-${ANTARES_TARGET}")
OMESSAGE("A New Tool for Adequacy Reporting of Electric Systems (RTE France)")
OMESSAGE_BOLD("Configuring...")
OMESSAGE("")

include("cmake/changelog.cmake")
include("cmake/checks.cmake")



if (ANTARES_LICENSE)
	OMESSAGE("{antares}  License : ${ANTARES_LICENSE}")
endif()

set(ANTARES_PRG_VERSION  "${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}")

if ((WIN32 OR WIN64) AND (NOT MINGW AND NOT MSVC AND NOT CYGWIN AND NOT MSYS))
	set(MSVC 1)
	set(ICC 1)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	OMESSAGE(" Enabled 64bits instructions sets")
	set(ANTARES_x86_64 true)
	set(ANTARES_INSTALLER_ARCH "64bits")
	set(ANTARES_INSTALLER_REDIST_ARCH "x64")
	set(ANTARES_MANIFEST_ARCH "ia64")
else()
	set(ANTARES_INSTALLER_ARCH "32bits")
	set(ANTARES_INSTALLER_REDIST_ARCH "x86")
	set(ANTARES_MANIFEST_ARCH "x86")
endif()

if (WIN32)
	if(MSVC)
		set(COMPILER_NAME "vc14")
		set(COMPILER_LIB_INCLUDE "vc")
	else(MSVC)
		set(COMPILER_NAME "gcc4.x")
		set(COMPILER_LIB_INCLUDE "gcc")
	endif(MSVC)
endif(WIN32)

#
# Beta
#
if(NOT ANTARES_BETA EQUAL 0)
	set(ANTARES_INSTALLER_BETA "-beta${ANTARES_BETA}")
else()
	set(ANTARES_INSTALLER_BETA "")
endif()

if(${ANTARES_RC})
	set(ANTARES_INSTALLER_RC "rc${ANTARES_RC}")
else()
	set(ANTARES_INSTALLER_RC "")
endif()

OMESSAGE("")
OMESSAGE("")

set(ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")


#
# Build Configuration
#
if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
	OMESSAGE("{antares} Build Configuration: RELEASE")
else()
	OMESSAGE("{antares} Build Configuration: DEBUG")
endif()




#
# Yuni Framework
#
if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
	set(YUNI_TARGET_MODE "release")
	set(ANTARES_VERSION_TARGET  "release")
else()
	set(YUNI_TARGET_MODE "debug")
	set(ANTARES_VERSION_TARGET  "debug")
endif()

# All libraries will be stored in /bin directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${YUNI_TARGET_MODE})



#
# SQLite
#
# SQLite is compiled from the repository
#add_subdirectory("ext/sqlite/src")



# Standard Settings
include(cmake/common-settings.cmake)


# Licence stuff
include(cmake/libcurl_openssl.cmake)

# Yuni Framework
configure_file("cmake/ProfileBuild.template.cmake" "ext/yuni/src/ProfileBuild.cmake")
add_subdirectory("ext/yuni/src")

#
# The Global Config.h
#
configure_file("config.h.cmake" "config.h")

OMESSAGE("") # empty line

# Sub Directories
add_subdirectory(libs)
add_subdirectory(internet)
add_subdirectory(ui)
add_subdirectory(solver)
add_subdirectory(analyzer)

#if (NOT WIN32 AND NOT WIN64)
#	add_subdirectory(script)
#endif()


# Tools
add_subdirectory(tools)

OMESSAGE("")

# Informations for NSIS
if(WIN32 OR WIN64)
	if(MSVC)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
			set(NSIS_TARGET "Release")
		else("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
			set(NSIS_TARGET "Debug")
		endif("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
		set(COMPILER_MARK "v")
		#set(COMPILER_INCLUDE "vs9")
	else(MSVC)
		set(NSIS_TARGET "")
		set(COMPILER_MARK "m")
		set(COMPILER_INCLUDE "mingw")
	endif(MSVC)
	configure_file("distrib/win32/version.cmake" "distrib/win32/version.nsh")
	configure_file("distrib/win32/build.template.cmake" "distrib/win32/build.nsh")
	configure_file("distrib/win32/make-zip-from-installer.cmake" "distrib/win32/make-zip-from-installer.bat")
else()
	configure_file("distrib/unix/packages.cmake" "distrib/unix/packages.sh")
endif()

