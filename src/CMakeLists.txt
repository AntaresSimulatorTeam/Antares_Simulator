cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

# Version
set(ANTARES_VERSION_HI 10)
set(ANTARES_VERSION_LO 0)
set(ANTARES_VERSION_REVISION 1)

# Beta release
set(ANTARES_BETA 0)
set(ANTARES_RC 0)

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

# GNU ld is single-pass for static libraries: circular dependencies between
# libraries (e.g. antares-solver-simulation <-> model_antares) cause unresolved
# symbols. --start-group/--end-group tells ld to re-scan until all are resolved.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options("LINKER:--start-group")
endif()

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

# When using a pre-compiled OR-Tools package, its transitive dependencies
# (absl, re2) must come from the same installation prefix. The vcpkg toolchain
# prepends vcpkg paths to CMAKE_PREFIX_PATH, so without intervention,
# find_dependency(absl) inside ortoolsConfig.cmake resolves to vcpkg's absl
# (a different version than what OR-Tools was compiled against), causing
# linker errors due to ABI incompatibility (different inline namespaces:
# e.g. absl::lts_20250512 vs absl::lts_20240722).
#
# Fix: locate the OR-Tools install prefix (from ortools_DIR or CMAKE_PREFIX_PATH)
# and set absl_DIR/re2_DIR to the co-installed versions before find_package(ortools)
# triggers find_dependency(absl). Recompute this even when absl_DIR/re2_DIR are
# already cached: switching branches with a reused build directory can leave
# stale paths behind, and CMake will otherwise keep mixing headers/libs from
# different Abseil installations.
# Note: GNUInstallDirs uses "lib" on Windows/Ubuntu but may use "lib64" on
# some 64-bit Linux distributions (e.g. Oracle Linux 8).
    # Candidate lib directory names (GNUInstallDirs may use lib or lib64)
set(_lib_dirs lib lib64)

# Determine OR-Tools prefix: either from explicit ortools_DIR or CMAKE_PREFIX_PATH
set(_ortools_prefix "")
if (DEFINED ortools_DIR)
    get_filename_component(_ortools_prefix "${ortools_DIR}/../../.." ABSOLUTE)
else ()
    foreach(_prefix IN LISTS CMAKE_PREFIX_PATH)
        foreach(_libdir IN LISTS _lib_dirs)
            if (EXISTS "${_prefix}/${_libdir}/cmake/ortools/ortoolsConfig.cmake")
                set(_ortools_prefix "${_prefix}")
                break()
            endif ()
        endforeach ()
        if (_ortools_prefix)
            break()
        endif ()
    endforeach ()
endif ()

if (_ortools_prefix)
    foreach(_libdir IN LISTS _lib_dirs)
        if (EXISTS "${_ortools_prefix}/${_libdir}/cmake/absl/abslConfig.cmake")
            set(_ortools_absl_dir "${_ortools_prefix}/${_libdir}/cmake/absl")
            if (NOT DEFINED absl_DIR OR NOT absl_DIR STREQUAL "${_ortools_absl_dir}")
                set(absl_DIR "${_ortools_absl_dir}" CACHE PATH
                    "Path to absl CMake config (co-installed with OR-Tools)" FORCE)
                message(STATUS "Setting absl_DIR to OR-Tools co-installed version: ${absl_DIR}")
            endif ()
        endif ()
        if (EXISTS "${_ortools_prefix}/${_libdir}/cmake/re2/re2Config.cmake")
            set(_ortools_re2_dir "${_ortools_prefix}/${_libdir}/cmake/re2")
            if (NOT DEFINED re2_DIR OR NOT re2_DIR STREQUAL "${_ortools_re2_dir}")
                set(re2_DIR "${_ortools_re2_dir}" CACHE PATH
                    "Path to re2 CMake config (co-installed with OR-Tools)" FORCE)
                message(STATUS "Setting re2_DIR to OR-Tools co-installed version: ${re2_DIR}")
            endif ()
        endif ()
    endforeach ()

    # Ensure OR-Tools' absl headers are found BEFORE vcpkg headers.
    # This is necessary because vcpkg.cmake (line 425-426) reverses the
    # CMAKE_PREFIX_PATH order for Debug builds, causing vcpkg's absl
    # (different ABI version) to be found instead of OR-Tools' co-installed absl.
    set(_absl_include "${_ortools_prefix}/include")
    if (EXISTS "${_absl_include}/absl")
        include_directories(BEFORE "${_absl_include}")
        message(STATUS "Prepending include dir for OR-Tools absl: ${_absl_include}")
    endif ()
endif ()
unset(_ortools_absl_dir)
unset(_ortools_re2_dir)
unset(_ortools_prefix)
unset(_lib_dirs)

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
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options("LINKER:--end-group")
endif()
