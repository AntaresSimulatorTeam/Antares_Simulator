cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable

# Version
set(ANTARES_VERSION_HI 9)
set(ANTARES_VERSION_LO 3)
set(ANTARES_VERSION_REVISION 5)

# Beta release
set(ANTARES_BETA 0)
set(ANTARES_RC 0)

set(ANTARES_VERSION_YEAR 2025)

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

# Sanitizers and strict warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # Strict warnings
    add_compile_options(-Wall -Wextra -Wshadow -Wconversion)
endif ()

# Sanitizers options (defined once)
option(ENABLE_ASAN "Enable AddressSanitizer (ASan)" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (UBSan)" OFF)

set(_antares_sanitizer_target_defined FALSE)
if ((ENABLE_ASAN OR ENABLE_UBSAN) AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU"))
    # Build sanitizer list
    set(_san_list "")
    if (ENABLE_ASAN)
        list(APPEND _san_list address)
    endif ()
    if (ENABLE_UBSAN)
        list(APPEND _san_list undefined)
    endif ()
    string(REPLACE ";" "," _san_flags "${_san_list}")

    message(STATUS "Sanitizers enabled: ${_san_flags}")

    # Create an INTERFACE target that carries sanitizer compile/link options.
    # This avoids applying flags globally (which would affect external dependencies).
    add_library(antares_sanitizers INTERFACE)
    target_compile_options(antares_sanitizers INTERFACE -fsanitize=${_san_flags} -fno-omit-frame-pointer)
    if (ENABLE_UBSAN)
        target_compile_options(antares_sanitizers INTERFACE -fno-sanitize-recover=undefined)
    endif ()

    if (COMMAND target_link_options)
        target_link_options(antares_sanitizers INTERFACE -fsanitize=${_san_flags})
    else ()
        # Fallback for older CMake: pass linker flags via INTERFACE_LINK_LIBRARIES as raw flags
        target_link_libraries(antares_sanitizers INTERFACE "-fsanitize=${_san_flags}")
    endif ()

    set(_antares_sanitizer_target_defined TRUE)
endif ()

# Clang-Tidy integration
option(ENABLE_CLANG_TIDY "Run clang-tidy static analysis during builds" ON)
find_program(CLANG_TIDY_EXE NAMES clang-tidy clang-tidy-18 clang-tidy-17)
set(_clang_tidy_available FALSE)
set(_clang_tidy_args "-warnings-as-errors=*")
if (ENABLE_CLANG_TIDY)
    if (CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(_clang_tidy_available TRUE)
    else ()
        message(WARNING "ENABLE_CLANG_TIDY is ON but clang-tidy was not found. Static analysis will be skipped.")
    endif ()
endif ()

# Note: global application of sanitizer flags was removed. The INTERFACE target
# 'antares_sanitizers' defined above is linked only to project targets, so
# external/imported dependencies are not affected.

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

option(BUILD_UI "Build antares ui simulator" OFF)
message(STATUS "Build antares ui simulator: ${BUILD_UI}")

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

#wxWidget not needed for all library find is done in ui CMakeLists.txt
if (VCPKG_TOOLCHAIN AND NOT BUILD_wxWidgets)
    #Add cmake directory to CMAKE_MODULE_PATH to use specific FindwxWidgets package needed for vcpkg
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wxWidgets")
endif ()

# TODO : review Standard Settings
include(cmake/common-settings.cmake)

# Default: disable UI build per request
set(BUILD_UI_DEFAULT OFF)
option(BUILD_UI "Build antares ui simulator" ${BUILD_UI_DEFAULT})
message(STATUS "Build antares ui simulator: ${BUILD_UI}")

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

#wxWidget not needed for all library find is done in ui CMakeLists.txt
if (VCPKG_TOOLCHAIN AND NOT BUILD_wxWidgets)
    #Add cmake directory to CMAKE_MODULE_PATH to use specific FindwxWidgets package needed for vcpkg
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/wxWidgets")
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

# Sub Directories
if (BUILD_UI)
    add_compile_definitions(BUILD_UI)
    add_subdirectory(ui)  #all antares ui libs + antares simulator
endif ()


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

# If we defined the antares_sanitizers interface target above, link it to all
# non-imported, non-interface, non-object targets created by this project.
# This ensures sanitizers are applied only to project targets and not to
# imported external dependencies.
if (_antares_sanitizer_target_defined)
    get_property(_all_targets GLOBAL PROPERTY TARGETS)
    foreach(_tgt IN LISTS _all_targets)
        if (_tgt STREQUAL "antares_sanitizers")
            continue()
        endif()
        # Skip imported targets
        get_target_property(_is_imported ${_tgt} IMPORTED)
        if (_is_imported)
            continue()
        endif()
        # Skip interface and object libraries
        get_target_property(_tgt_type ${_tgt} TYPE)
        if (_tgt_type STREQUAL "INTERFACE_LIBRARY" OR _tgt_type STREQUAL "OBJECT_LIBRARY")
            continue()
        endif()
        # Try to link sanitizer interface. Use TARGET check to avoid errors.
        if (TARGET ${_tgt})
            message(VERBOSE "Applying sanitizers to target: ${_tgt}")
            target_link_libraries(${_tgt} PRIVATE antares_sanitizers)
        endif()
    endforeach()
endif()

# If clang-tidy is available, apply it per-target to avoid running it on external/third-party code (e.g. ext/)
if (_clang_tidy_available)
    # Build the final clang-tidy property string used by CMake targets
    set(_clang_tidy_property "${CLANG_TIDY_EXE};${_clang_tidy_args};--extra-arg-before=--driver-mode=g++")

    get_property(_all_targets GLOBAL PROPERTY TARGETS)
    foreach(_tgt IN LISTS _all_targets)
        # Skip the sanitizer interface and any non-real targets
        if (_tgt STREQUAL "antares_sanitizers")
            continue()
        endif()

        # Skip imported targets
        get_target_property(_is_imported ${_tgt} IMPORTED)
        if (_is_imported)
            continue()
        endif()

        # Skip interface and object libraries
        get_target_property(_tgt_type ${_tgt} TYPE)
        if (_tgt_type STREQUAL "INTERFACE_LIBRARY" OR _tgt_type STREQUAL "OBJECT_LIBRARY")
            continue()
        endif()

        # Get sources for the target (may be empty)
        get_target_property(_srcs ${_tgt} SOURCES)
        if (NOT _srcs)
            # No sources to analyze
            continue()
        endif()

        # Skip targets that include sources under the ext/ directory (third-party)
        set(_has_ext_src FALSE)
        foreach(_s IN LISTS _srcs)
            # Only analyze sources with absolute or relative paths; check if path contains '/ext/'
            string(FIND "${_s}" "/ext/" _found)
            if (_found GREATER -1)
                set(_has_ext_src TRUE)
                break()
            endif()
        endforeach()
        if (_has_ext_src)
            message(VERBOSE "Skipping clang-tidy for target ${_tgt} (external sources detected)")
            continue()
        endif()

        # Finally, set clang-tidy property on the target
        if (TARGET ${_tgt})
            message(VERBOSE "Setting CXX_CLANG_TIDY for target ${_tgt}")
            set_property(TARGET ${_tgt} PROPERTY CXX_CLANG_TIDY "${_clang_tidy_property}")
        endif()
    endforeach()
endif()

# Informations for NSIS
if (WIN32 OR WIN64)
    if (MSVC)
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(NSIS_TARGET "Release")
        else ()
            set(NSIS_TARGET "Debug")
        endif ()
        set(COMPILER_MARK "v")
        #set(COMPILER_INCLUDE "vs9")
    else (MSVC)
        set(NSIS_TARGET "")
        set(COMPILER_MARK "m")
        set(COMPILER_INCLUDE "mingw")
    endif (MSVC)

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
    include(InstallRequiredSystemLibraries)

    #Convert to native and add double quote to be used in NSIS package
    foreach (SYSTEM_RUNTIME_LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        file(TO_NATIVE_PATH ${SYSTEM_RUNTIME_LIB} SYSTEM_RUNTIME_LIB_NATIVE)

        list(APPEND SYSTEM_RUNTIME_LIBS_NATIVE \"${SYSTEM_RUNTIME_LIB_NATIVE}\")
    endforeach ()

    #Replace ; by space so SYSTEM_RUNTIME_LIBS_STR can be used in File function in NSIS file
    string(REPLACE ";" " " SYSTEM_RUNTIME_LIBS_STR "${SYSTEM_RUNTIME_LIBS_NATIVE}")

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


else ()

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

    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libwxgtk3.0-gtk3-0v5 | libwxgtk3.2-dev")
    set(CPACK_RPM_PACKAGE_REQUIRES "wxGTK3")
    set(CPACK_RPM_PACKAGE_AUTOREQPROV "0")

    configure_file("distrib/unix/packages.cmake" "distrib/unix/packages.sh")


endif ()

# Load packaging facilities.
include(CPack)

#Last. We need all target defined
add_subdirectory(packaging)
