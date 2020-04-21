project(antares)
cmake_minimum_required(VERSION 3.0)

# Version
set(ANTARES_VERSION_HI	      7)
set(ANTARES_VERSION_LO	      1)
set(ANTARES_VERSION_REVISION  0)
set(ANTARES_VERSION_YEAR      2019)

set(ANTARES_PUBLISHER	   "RTE France")
set(ANTARES_WEBSITE	   "https://antares.rte-france.com")

# Beta release
set(ANTARES_BETA 0)
#set(ANTARES_RC 1)

# CMAKE - Suppress useless features
set(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES true)
set(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES true)

include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)

#CMake Macro to display message
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

# Copy changelog
include("cmake/changelog.cmake")

#Check compiler version
include("cmake/checks.cmake")


if (ANTARES_LICENSE)
	OMESSAGE("{antares}  License : ${ANTARES_LICENSE}")
endif()

set(ANTARES_PRG_VERSION  "${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}")

#TODO : define if MSVC is used (check if can be removed)
if ((WIN32 OR WIN64) AND (NOT MINGW AND NOT MSVC AND NOT CYGWIN AND NOT MSYS))
	set(MSVC 1)
	set(ICC 1)
endif()

#Define which version is compiled (32bits or 64bits)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	OMESSAGE(" Enabled 64bits instructions sets")
	set(ANTARES_x86_64 			true)
	set(ANTARES_INSTALLER_ARCH 		"64bits")
	set(ANTARES_INSTALLER_REDIST_ARCH 	"x64")
	set(ANTARES_MANIFEST_ARCH 		"ia64")
else()
	set(ANTARES_INSTALLER_ARCH 		"32bits")
	set(ANTARES_INSTALLER_REDIST_ARCH 	"x86")
	set(ANTARES_MANIFEST_ARCH 		"x86")
endif()


#TODO : should not defined compiler name with a global variable
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

#Display Build Configuration (debug or release)
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

# TODO : removed to be confirmed : old fashion CMake All libraries will be stored in /bin directory
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${YUNI_TARGET_MODE})


# Define needed external library

#libcurl
find_package(CURL REQUIRED)

#openssl	
find_package(OpenSSL REQUIRED)

#wxWidget not needed for all library find is done in ui CMakeLists.txt

# TODO : review Standard Settings
include(cmake/common-settings.cmake)

# Configure config.h with current ANTARES version
configure_file("config.h.cmake" "config.h")

# Yuni Framework
configure_file("cmake/ProfileBuild.template.cmake" "ext/yuni/src/ProfileBuild.cmake")
add_subdirectory("ext/yuni/src")

#Sirius solver
add_subdirectory("ext/Sirius_Solver" "ext/Sirius_Solver")

OMESSAGE("") # empty line

# Sub Directories
add_subdirectory(libs) 		#antares-core fswalker
add_subdirectory(internet) 	#libantares-license base64 keys
add_subdirectory(ui)		#all antares ui libs + antares simulator
add_subdirectory(solver)	#antares solver and all associated libs
add_subdirectory(analyzer)	#antares analyser

# Tools
add_subdirectory(tools)	#All antares tools

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

