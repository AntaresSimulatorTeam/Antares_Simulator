cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

# Version
set(ANTARES_VERSION_HI 10)
set(ANTARES_VERSION_LO 0)
set(ANTARES_VERSION_REVISION 0)

# Beta release
set(ANTARES_BETA 0)
set(ANTARES_RC 1)

set(ANTARES_VERSION_YEAR 2026)

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

set(CMAKE_INSTALL_RPATH $ORIGIN)

include(CheckCXXSourceCompiles)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckCXXCompilerFlag)
include(CMakeDependentOption)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

if (DEFINED VCPKG_ROOT)
    include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif ()


#CMake function
include("cmake/utils.cmake")

# Only used for code generation (optional)
set(ANTLR_JAR_PATH "" CACHE FILEPATH "Path to ANTLR 4 .jar file (e.g., antlr-4.13.2-complete.jar)")
message(STATUS "ANTLR_JAR_PATH is: ${ANTLR_JAR_PATH}")

#Generate ANTLR4 source files from grammar files
include("cmake/antlr4-generate.cmake")


#CMake Macro to display message
include("cmake/messages.cmake")

# If an ORTOOLS_DIR is provided (either cache var `ortools_DIR` or env var ORTOOLS_DIR),
# prefer the precompiled OR-Tools install and its bundled absl by adjusting
# `CMAKE_PREFIX_PATH` and setting `ortools_DIR`/`absl_DIR` so `find_package`
# resolves OR-Tools/absl from that install before vcpkg.
if(NOT DEFINED ortools_DIR AND DEFINED ENV{ORTOOLS_DIR})
    # Expect the env to point to the OR-Tools repo root used in CI; the install
    # lives under <ORTOOLS_DIR>/install
    set(_ortools_install_prefix "$ENV{ORTOOLS_DIR}/install")
    list(INSERT CMAKE_PREFIX_PATH 0 "${_ortools_install_prefix}")
    set(ortools_DIR "${_ortools_install_prefix}/lib/cmake/ortools" CACHE PATH "Precompiled OR-Tools CMake dir" FORCE)
    set(absl_DIR "${_ortools_install_prefix}/lib/cmake/absl" CACHE PATH "OR-Tools provided absl CMake dir" FORCE)
    message(STATUS "Using OR-Tools install from ENV{ORTOOLS_DIR}: ${_ortools_install_prefix}")
    message(STATUS "Set ortools_DIR=${ortools_DIR}")
    message(STATUS "Set absl_DIR=${absl_DIR}")
endif()

# If user passed ortools_DIR on the command line, derive absl_DIR from it when
# absl_DIR is not set so that OR-Tools' absl is preferred automatically.
if(DEFINED ortools_DIR)
    # ortools_DIR typically points to .../install/lib/cmake/ortools or .../lib/cmake/ortools
    get_filename_component(_cmake_dir "${ortools_DIR}" DIRECTORY)
    # _cmake_dir -> .../lib/cmake
    get_filename_component(_lib_dir "${_cmake_dir}" DIRECTORY)
    # _lib_dir -> .../lib
    # Prepend install prefix (one level up) to CMAKE_PREFIX_PATH to prioritize it
    get_filename_component(_install_prefix "${_lib_dir}" DIRECTORY)
    list(INSERT CMAKE_PREFIX_PATH 0 "${_install_prefix}")
    if(NOT DEFINED absl_DIR)
        set(absl_DIR "${_cmake_dir}/absl" CACHE PATH "Derived absl CMake dir from ortools_DIR" FORCE)
        message(STATUS "Derived absl_DIR=${absl_DIR} from ortools_DIR=${ortools_DIR}")
    endif()
    if(NOT DEFINED Protobuf_DIR)
        set(_maybe_protobuf_dir "${_cmake_dir}/protobuf")
        if(EXISTS "${_maybe_protobuf_dir}")
            set(Protobuf_DIR "${_maybe_protobuf_dir}" CACHE PATH "Derived Protobuf CMake dir from ortools_DIR" FORCE)
            message(STATUS "Derived Protobuf_DIR=${Protobuf_DIR} from ortools_DIR=${ortools_DIR}")
        else()
            message(STATUS "Could not derive Protobuf_DIR at ${_maybe_protobuf_dir}; you may need to set -DProtobuf_DIR if configure fails.")
        endif()
    endif()
endif()

set(ANTARES_VERSION_TAG "")

if (${ANTARES_BETA})
    set(ANTARES_VERSION_TAG "-beta${ANTARES_BETA}")
endif ()

if (${ANTARES_RC})
    set(ANTARES_VERSION_TAG "-rc${ANTARES_RC}")
endif ()

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
endif ()

set(ANTARES_PRG_VERSION "${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}")

#TODO : define if MSVC is used (check if can be removed)
if ((WIN32 OR WIN64) AND (NOT MINGW AND NOT MSVC AND NOT CYGWIN AND NOT MSYS))
    set(MSVC 1)
    set(ICC 1)
endif ()

#Define which version is compiled (32bits or 64bits)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    OMESSAGE(" Enabled 64bits instructions sets")
    set(ANTARES_x86_64 true)
    set(ANTARES_INSTALLER_ARCH "64bits")
    set(ANTARES_INSTALLER_REDIST_ARCH "x64")
    set(ANTARES_MANIFEST_ARCH "ia64")
else ()
    set(ANTARES_INSTALLER_ARCH "32bits")
    set(ANTARES_INSTALLER_REDIST_ARCH "x86")
    set(ANTARES_MANIFEST_ARCH "x86")
endif ()


#TODO : should not defined compiler name with a global variable
if (WIN32)
    if (MSVC)
        set(COMPILER_NAME "vc14")
        set(COMPILER_LIB_INCLUDE "vc")
    else (MSVC)
        set(COMPILER_NAME "gcc4.x")
        set(COMPILER_LIB_INCLUDE "gcc")
    endif (MSVC)
endif (WIN32)

#
# Beta
#
if (NOT ANTARES_BETA EQUAL 0)
    set(ANTARES_INSTALLER_BETA "-beta${ANTARES_BETA}")
else ()
    set(ANTARES_INSTALLER_BETA "")
endif ()

if (${ANTARES_RC})
    set(ANTARES_INSTALLER_RC "-rc${ANTARES_RC}")
else ()
    set(ANTARES_INSTALLER_RC "")
endif ()

OMESSAGE("")
OMESSAGE("")

#Display Build Configuration (debug or release)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    OMESSAGE("{antares} Build Configuration: RELEASE")
else ()
    OMESSAGE("{antares} Build Configuration: DEBUG")
endif ()

#
# Yuni Framework
#
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(YUNI_TARGET_MODE "release")
    set(ANTARES_VERSION_TARGET "release")
else ()
    set(YUNI_TARGET_MODE "debug")
    set(ANTARES_VERSION_TARGET "debug")
endif ()


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

option(BUILD_TOOLS "Build antares tools" OFF)
message(STATUS "Build antares tools: ${BUILD_TOOLS}")

option(BUILD_ORTOOLS "Build OR-Tools" OFF)
message(STATUS "Build OR-Tools: ${BUILD_ORTOOLS}")

# When using a precompiled OR-Tools bundle, prefer the libraries shipped with
# that bundle (absl, protobuf, etc). New option `ORTOOLS_INSTALL_DIR` accepts
# the install prefix (the directory that contains `lib/` and `include/`). If
# provided, we set `ortools_DIR`, `absl_DIR` and `Protobuf_DIR` to the
# corresponding cmake config directories from that install. This avoids using
# `CMAKE_PREFIX_PATH` globally which can interfere with vcpkg resolution.
option(ORTOOLS_INSTALL_DIR "Path to a precompiled OR-Tools install prefix" "" CACHE PATH "Precompiled OR-Tools install prefix (contains lib/ include/)")
if(ORTOOLS_INSTALL_DIR)
    # Normalize: allow user to pass either the install root or the pack's lib/cmake/ortools
    if(EXISTS "${ORTOOLS_INSTALL_DIR}/lib/cmake/ortools")
        set(_ortools_cmake_dir "${ORTOOLS_INSTALL_DIR}/lib/cmake/ortools")
        set(_install_prefix "${ORTOOLS_INSTALL_DIR}")
    elseif(EXISTS "${ORTOOLS_INSTALL_DIR}")
        # If the path looks like .../lib/cmake/ortools
        get_filename_component(_maybe ${ORTOOLS_INSTALL_DIR} REALPATH)
        if(EXISTS "${_maybe}/lib/cmake/ortools")
            set(_ortools_cmake_dir "${_maybe}/lib/cmake/ortools")
            set(_install_prefix "${_maybe}")
        else()
            # fallback: assume user passed the cmake dir directly
            set(_ortools_cmake_dir "${ORTOOLS_INSTALL_DIR}")
            get_filename_component(_install_prefix "${_ortools_cmake_dir}" DIRECTORY)
            get_filename_component(_install_prefix "${_install_prefix}" DIRECTORY)
        endif()
    endif()

    if(EXISTS "${_ortools_cmake_dir}")
        set(ortools_DIR "${_ortools_cmake_dir}" CACHE PATH "Precompiled OR-Tools CMake dir" FORCE)
        message(STATUS "Using OR-Tools install from ORTOOLS_INSTALL_DIR: ${_install_prefix}")
        message(STATUS "Set ortools_DIR=${ortools_DIR}")

        # Derive absl and protobuf config dirs from the same install prefix if present
        if(EXISTS "${_install_prefix}/lib/cmake/absl")
            set(absl_DIR "${_install_prefix}/lib/cmake/absl" CACHE PATH "OR-Tools provided absl CMake dir" FORCE)
            message(STATUS "Set absl_DIR=${absl_DIR}")
        endif()
        if(EXISTS "${_install_prefix}/lib/cmake/protobuf")
            set(Protobuf_DIR "${_install_prefix}/lib/cmake/protobuf" CACHE PATH "OR-Tools provided protobuf CMake dir" FORCE)
            message(STATUS "Set Protobuf_DIR=${Protobuf_DIR}")
        endif()
    else()
        message(WARNING "ORTOOLS_INSTALL_DIR was provided but could not be resolved: ${ORTOOLS_INSTALL_DIR}")
    endif()
endif()

option(BUILD_MERSENNE_TWISTER_PYBIND11 "Build pybind11 bindings for Mersenne-Twister" OFF)
if (${BUILD_MERSENNE_TWISTER_PYBIND11})
    find_package(pybind11 REQUIRED)
endif ()

#Boost header libraries
find_package(Boost REQUIRED)

find_package(antlr4-runtime REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(fmt REQUIRED)

#Sirius solver
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif ()

find_package(sirius_solver REQUIRED)

find_package(ortools)
if (NOT ortools_FOUND OR BUILD_ORTOOLS)
    message(STATUS "OR-Tools tag ${ORTOOLS_TAG}")
    FetchContent_Declare(ortools-rte
            GIT_REPOSITORY "https://github.com/rte-france/or-tools-rte.git"
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
    set(ortools_REPO "https://github.com/google/or-tools.git")
    string(REGEX MATCH "^[^-]+" OFFICIAL_TAG ${ORTOOLS_TAG})
    set(ortools_REF ${OFFICIAL_TAG})

    FetchContent_MakeAvailable(ortools-rte)
endif ()

find_package(minizip-ng QUIET)
if (minizip-ng_FOUND)
    add_library(MINIZIP::minizip ALIAS MINIZIP::minizip-ng)
else ()
    find_package(minizip)
    if (minizip_FOUND)
        message(STATUS "Found minizip (not minizip-ng).")
    else ()
        message(FATAL_ERROR "Minizip not found.")
    endif ()
endif ()

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

add_subdirectory(api)
add_subdirectory(io)
add_subdirectory(libs)   #antares-core fswalker

add_subdirectory(expressions)   #modeler expressions (AST)
add_subdirectory(optimisation)  #optimisation APIs
add_subdirectory(study)         #antares study model
add_subdirectory(solver)     #antares solver and all associated libs
add_subdirectory(modeler)       #antares modeler
add_subdirectory(modeler-optimisation-container)       #antares modeler

# Tools
if (BUILD_TOOLS)
    add_subdirectory(tools) #All antares tools
endif ()

# Tests
# 	Not setting BUILD_TESTING as a command line argument is equivalent to use -DBUILD_TESTING=OFF
# 	To build tests, use -DBUILD_TESTING=ON
option(BUILD_TESTING "Activates unit tests building" OFF)
if (BUILD_TESTING)
    # include(CTest) # calls enable_testing() and turns BUILD_TESTING to ON by default (among other useful things)
    enable_testing()

    add_subdirectory(tests)
endif ()

OMESSAGE("")


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    get_linux_lsb_release_information()
    message(STATUS "Linux ${LSB_RELEASE_ID_SHORT} ${LSB_RELEASE_VERSION_SHORT} ${LSB_RELEASE_CODENAME_SHORT}")
    set(CPACK_SYSTEM_NAME "${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_VERSION_SHORT}")
endif ()

#For now only test deb, need to define several CPACK variable
set(CPACK_PROJECT_NAME "antares-deb-test")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "RTE")
set(CPACK_DEBIAN_PACKAGE_NAME "antares")
set(CPACK_PACKAGE_VERSION_MAJOR ${ANTARES_VERSION_HI})
set(CPACK_PACKAGE_VERSION_MINOR ${ANTARES_VERSION_LO})
set(CPACK_PACKAGE_VERSION_PATCH ${ANTARES_VERSION_REVISION}${ANTARES_VERSION_TAG})

configure_file("distrib/unix/packages.cmake" "distrib/unix/packages.sh")

# Load packaging facilities.
include(CPack)

#Last. We need all target defined
add_subdirectory(packaging)
