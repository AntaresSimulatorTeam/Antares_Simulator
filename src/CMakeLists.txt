cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

# Version
set(ANTARES_VERSION_HI	      9)
set(ANTARES_VERSION_LO	      1)
set(ANTARES_VERSION_REVISION  0)

# Beta release
set(ANTARES_BETA 0)
set(ANTARES_RC 0)

set(ANTARES_VERSION_YEAR      2024)

project(antares
        VERSION ${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}.${ANTARES_VERSION_REVISION})

set(ANTARES_PUBLISHER "RTE France")
set(ANTARES_WEBSITE "https://antares-simulator.org/")
set(ANTARES_ONLINE_DOC "https://antares-simulator.readthedocs.io/")


# OR-Tools tag
file(READ "../ortools_tag" ORTOOLS_TAG)

# CMAKE - Suppress useless features
set(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES true)
set(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES true)

set(CMAKE_INSTALL_RPATH 		$ORIGIN)

include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

if (DEFINED VCPKG_ROOT)
    include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()


#CMake function
include("cmake/utils.cmake")

#CMake Macro to display message
include("cmake/messages.cmake")

set(ANTARES_VERSION_TAG "")

if(${ANTARES_BETA})
	set(ANTARES_VERSION_TAG "-beta${ANTARES_BETA}")
endif()

if(${ANTARES_RC})
	set (ANTARES_VERSION_TAG "-rc${ANTARES_RC}")
endif()

# git SHA-1
find_package(Git QUIET)
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --abbrev=7 --always --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1_SHORT
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# Build Configuration
set(ANTARES_TARGET "${CMAKE_BUILD_TYPE}")

OMESSAGE("")
OMESSAGE_TITLE("Antares " "v${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}.${ANTARES_VERSION_REVISION}${ANTARES_VERSION_TAG}-${ANTARES_TARGET}")
OMESSAGE("A New Tool for Adequacy Reporting of Electric Systems (RTE France)")
OMESSAGE_BOLD("Configuring...")
OMESSAGE("")

# Making gatherings of several projects under a project folder in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
	set(ANTARES_INSTALLER_RC "-rc${ANTARES_RC}")
else()
	set(ANTARES_INSTALLER_RC "")
endif()

OMESSAGE("")
OMESSAGE("")

#Display Build Configuration (debug or release)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	OMESSAGE("{antares} Build Configuration: RELEASE")
else()
	OMESSAGE("{antares} Build Configuration: DEBUG")
endif()

#
# Yuni Framework
#
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(YUNI_TARGET_MODE "release")
	set(ANTARES_VERSION_TARGET  "release")
else()
	set(YUNI_TARGET_MODE "debug")
	set(ANTARES_VERSION_TARGET  "debug")
endif()


set(CODE_COVERAGE FALSE CACHE BOOL "Enable code coverage")
if (CODE_COVERAGE)
	# if code-coverage is ON, force tests build
	set(BUILD_TESTING ON)

	include("cmake/CodeCoverage.cmake")
	code_coverage(NAME code-coverage
			OUTPUT_DIR coverage
			EXCLUDE_DIRS tests)
endif ()

# TODO : removed to be confirmed : old fashion CMake All libraries will be stored in /bin directory
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${YUNI_TARGET_MODE})

option(BUILD_UI "Build antares ui simulator" ON)
message(STATUS "Build antares ui simulator: ${BUILD_UI}")

option(BUILD_TOOLS "Build antares tools" OFF)
message(STATUS "Build antares tools: ${BUILD_TOOLS}")

option(BUILD_ORTOOLS "Build OR-Tools" OFF)
message(STATUS "Build OR-Tools: ${BUILD_ORTOOLS}")

option(BUILD_MINIZIP "Build minizip" OFF)
message(STATUS "Build minizip: ${BUILD_MINIZIP}")

option(BUILD_MERSENNE_TWISTER_PYBIND11 "Build pybind11 bindings for Mersenne-Twister" OFF)
if (${BUILD_MERSENNE_TWISTER_PYBIND11})
    find_package(pybind11 REQUIRED)
endif()

#Define install directory
if (NOT DEPS_INSTALL_DIR)
    SET(DEPS_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../rte-antares-deps-${CMAKE_BUILD_TYPE})

else()
    if(NOT IS_ABSOLUTE ${DEPS_INSTALL_DIR})
        SET(DEPS_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${DEPS_INSTALL_DIR})
    endif()    
endif()


# Add DEPS_INSTALL_DIR with antares-xpansion CMAKE_BUILD_TYPE
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

	string(REPLACE ${CMAKE_BUILD_TYPE} Release
			DEPS_INSTALL_DIR_XPANSION
			${DEPS_INSTALL_DIR})

	list(APPEND CMAKE_PREFIX_PATH ${DEPS_INSTALL_DIR_XPANSION})
	
elseif("${CMAKE_BUILD_TYPE}" STREQUAL  "debug")

	string(REPLACE ${CMAKE_BUILD_TYPE} Debug
			DEPS_INSTALL_DIR_XPANSION
			${DEPS_INSTALL_DIR})
	
	list(APPEND CMAKE_PREFIX_PATH ${DEPS_INSTALL_DIR_XPANSION})

endif()

list(APPEND CMAKE_PREFIX_PATH ${DEPS_INSTALL_DIR})
message(STATUS "CMAKE_PREFIX_PATH : ${CMAKE_PREFIX_PATH}")

# Build Needed dependencies
add_subdirectory(antares-deps)

#Add needed definition in case of external dependencies compilation
include (antares-deps/cmake/additionnal-definitions.cmake)

#Boost header libraries
find_package(Boost REQUIRED)

#TODO : Add ZLIB if  VCPKG used why is this needed
if (VCPKG_TOOLCHAIN) 
    #zlib
    find_package(ZLIB REQUIRED)
endif()

#Sirius solver
if(POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW)
endif()

if (VCPKG_TOOLCHAIN)
	list(APPEND CMAKE_PREFIX_PATH "${sirius_solver_ROOT}")
	list(APPEND CMAKE_PREFIX_PATH "${ortools_ROOT}")
endif()

find_package(sirius_solver)

if (NOT sirius_solver_FOUND)
	message (FATAL_ERROR "Sirius solver not found. Sirius solver can be compiled with -DBUILD_sirius=ON or you can specify previous dependency install directory with -DCMAKE_PREFIX_PATH or -DDEPS_INSTALL_DIR")
endif()

#gflags needed for ortools
set(GFLAGS_USE_TARGET_NAMESPACE TRUE) 
find_package(gflags)

find_package(ortools)
if(NOT ortools_FOUND OR BUILD_ORTOOLS)
message(STATUS "OR-Tools tag ${ORTOOLS_TAG}")
  FetchContent_Declare(ortools
    GIT_REPOSITORY "https://github.com/rte-france/or-tools"
    GIT_TAG ${ORTOOLS_TAG}
    GIT_SHALLOW TRUE
    )

  # Pass options to OR-Tools's CMake
  set(BUILD_DEPS "ON" CACHE INTERNAL "")
  set(BUILD_SAMPLES "OFF" CACHE INTERNAL "")
  set(BUILD_FLATZINC "OFF" CACHE INTERNAL "")
  set(BUILD_EXAMPLES "OFF" CACHE INTERNAL "")
  set(USE_SCIP "ON" CACHE INTERNAL "")
  set(USE_GLPK "ON" CACHE INTERNAL "")
  # We build OR-Tools as a static lib. Cyclic dependencies are detected
  # without this flag.
  set(BUILD_SHARED_LIBS "OFF" CACHE INTERNAL "")
  # In mode optimization error analysis, we call Sirius through or-tools
  # So we need to activate Sirius in or-tools configuration (OFF by default)
  set(USE_SIRIUS "ON" CACHE INTERNAL "")

  FetchContent_MakeAvailable(ortools)
endif()

find_package(minizip QUIET)

if(NOT minizip_FOUND OR BUILD_MINIZIP)
	if (NOT minizip_FOUND)
		message("minizip not found, downloading")
	endif ()
	if (BUILD_MINIZIP)
		message("BUILD_MINIZIP set, downloading")
	endif ()
    # Repository + tag
    set(MZ_REPOSITORY "https://github.com/zlib-ng/minizip-ng.git")
    set(MZ_TAG "4.0.1")
    # CMake flags
    set(MZ_LZMA "OFF" CACHE INTERNAL "")
    set(MZ_ZSTD "OFF" CACHE INTERNAL "")
    set(MZ_BZIP2 "OFF" CACHE INTERNAL "")
    set(MZ_PKCRYPT "OFF" CACHE INTERNAL "")
    set(MZ_WZAES "OFF" CACHE INTERNAL "")
    set(MZ_OPENSSL "OFF" CACHE INTERNAL "")
    set(MZ_ICONV "OFF" CACHE INTERNAL "")

	FetchContent_Declare(minizip
		GIT_REPOSITORY ${MZ_REPOSITORY}
		GIT_TAG ${MZ_TAG}
		OVERRIDE_FIND_PACKAGE
	)

	FetchContent_MakeAvailable(minizip)
endif()
find_package(minizip REQUIRED)

#wxWidget not needed for all library find is done in ui CMakeLists.txt
if (VCPKG_TOOLCHAIN AND NOT BUILD_wxWidgets) 
    #Add cmake directory to CMAKE_MODULE_PATH to use specific FindwxWidgets package needed for vcpkg
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wxWidgets")
endif()

# TODO : review Standard Settings
include(cmake/common-settings.cmake)

add_subdirectory(config)

# Configure config.h with current ANTARES version
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in"
		"${CMAKE_CURRENT_SOURCE_DIR}/config/include/antares/config/config.h")

# Yuni Framework
configure_file("cmake/ProfileBuild.template.cmake" "ext/yuni/src/ProfileBuild.cmake")
add_subdirectory("ext/yuni/src")

OMESSAGE("") # empty line

# Sub Directories
add_subdirectory(libs) 		#antares-core fswalker

if(BUILD_UI)
	add_subdirectory(ui)		#all antares ui libs + antares simulator
endif()

add_subdirectory(solver)	#antares solver and all associated libs
add_subdirectory(analyzer)	#antares analyser

# Tools
if (BUILD_TOOLS)
add_subdirectory(tools)	#All antares tools
endif()

# Tests
# 	Not setting BUILD_TESTING as a command line argument is equivalent to use -DBUILD_TESTING=OFF
# 	To build tests, use -DBUILD_TESTING=ON
option(BUILD_TESTING "Activates unit tests building" OFF)
if(BUILD_TESTING)
	# include(CTest) # calls enable_testing() and turns BUILD_TESTING to ON by default (among other useful things)
	enable_testing()

	add_subdirectory(tests)
endif()

OMESSAGE("")



  
# Informations for NSIS
if(WIN32 OR WIN64)
	if(MSVC)
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
			set(NSIS_TARGET "Release")
		else()
			set(NSIS_TARGET "Debug")
		endif()
		set(COMPILER_MARK "v")
		#set(COMPILER_INCLUDE "vs9")
	else(MSVC)
		set(NSIS_TARGET "")
		set(COMPILER_MARK "m")
		set(COMPILER_INCLUDE "mingw")
	endif(MSVC)
	
	set(CPACK_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/distrib/win32 ${CPACK_MODULE_PATH})

	# Configure file with custom definitions for NSIS.
	configure_file(
		${PROJECT_SOURCE_DIR}/distrib/win32/version.cmake
		${CMAKE_CURRENT_BINARY_DIR}/distrib/win32/version.nsh
	)
	
	configure_file(
		${PROJECT_SOURCE_DIR}/distrib/win32/build.template.cmake
		${CMAKE_CURRENT_BINARY_DIR}/distrib/win32/build.nsh
	)
	
	configure_file(
		${PROJECT_SOURCE_DIR}/distrib/win32/make-zip-from-installer.cmake
		${CMAKE_CURRENT_BINARY_DIR}/distrib/win32/make-zip-from-installer.bat
	)
	
	SET(ANTARES_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

	# Define system libraries so NSIS can package these files
	include (InstallRequiredSystemLibraries)

	#Convert to native and add double quote to be used in NSIS package
	foreach(SYSTEM_RUNTIME_LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
		file(TO_NATIVE_PATH ${SYSTEM_RUNTIME_LIB} SYSTEM_RUNTIME_LIB_NATIVE)

		list(APPEND SYSTEM_RUNTIME_LIBS_NATIVE \"${SYSTEM_RUNTIME_LIB_NATIVE}\")
	endforeach()

	#Replace ; by space so SYSTEM_RUNTIME_LIBS_STR can be used in File function in NSIS file
	string (REPLACE ";" " " SYSTEM_RUNTIME_LIBS_STR "${SYSTEM_RUNTIME_LIBS_NATIVE}")

	configure_file(
		${PROJECT_SOURCE_DIR}/distrib/win32/NSIS.template.in
		${CMAKE_CURRENT_BINARY_DIR}/distrib/win32/NSIS.template.in
		@ONLY
	)
	
	#Define cpack install script to checkout examples
	configure_file(
		${PROJECT_SOURCE_DIR}/distrib/CMakeLists.txt.in
		${CMAKE_CURRENT_BINARY_DIR}/distrib/CMakeLists.txt
		@ONLY
	)

	set(CPACK_INSTALL_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/distrib/CMakeLists.txt)

	#Set package name from CMake and antares variables
	set(CPACK_PACKAGE_FILE_NAME "rte-antares-${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}.${ANTARES_VERSION_REVISION}${ANTARES_VERSION_TAG}-installer-${ANTARES_INSTALLER_ARCH}")


else()

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        get_linux_lsb_release_information()
        message(STATUS "Linux ${LSB_RELEASE_ID_SHORT} ${LSB_RELEASE_VERSION_SHORT} ${LSB_RELEASE_CODENAME_SHORT}")
        set(CPACK_SYSTEM_NAME "${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_VERSION_SHORT}") 
    endif()

	#For now only test deb, need to define several CPACK variable
	set(CPACK_PROJECT_NAME "antares-deb-test")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "RTE")
	set(CPACK_DEBIAN_PACKAGE_NAME "antares")
	set(CPACK_PACKAGE_VERSION_MAJOR ${ANTARES_VERSION_HI})
	set(CPACK_PACKAGE_VERSION_MINOR ${ANTARES_VERSION_LO})
	set(CPACK_PACKAGE_VERSION_PATCH ${ANTARES_VERSION_REVISION}${ANTARES_VERSION_TAG})

	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libwxgtk3.0-gtk3-0v5 | libwxgtk3.2-dev")
    set(CPACK_RPM_PACKAGE_REQUIRES "wxGTK3")
    set(CPACK_RPM_PACKAGE_AUTOREQPROV "0")

	configure_file("distrib/unix/packages.cmake" "distrib/unix/packages.sh")


endif()

# Load packaging facilities.
include(CPack)

#Last. We need all target defined
add_subdirectory(packaging)
