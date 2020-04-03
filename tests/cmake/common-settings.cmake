
set(ANTARES_BIN "${ANTARES_ROOT}/bin")
set(UNIT_LIBRARIES_DIR "${ANTARES_TESTS}/unit-libs")


if(	NOT DEFINED CMAKE_BUILD_TYPE
	OR "${CMAKE_BUILD_TYPE}" STREQUAL ""
	OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug"
	OR "${CMAKE_BUILD_TYPE}" STREQUAL "debug")
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
elseif(	"${CMAKE_BUILD_TYPE}" STREQUAL "Release"
		OR "${CMAKE_BUILD_TYPE}" STREQUAL "release")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
else()
	message(FATAL_ERROR "common-settings - CMAKE_BUILD_TYPE variable must have value : empty, Debug, debug, Release, release")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(BUILD_TYPE_UP_CASE "Debug")
	set(BUILD_TYPE_LOW_CASE "debug")
else()
	set(BUILD_TYPE_UP_CASE "Release")
	set(BUILD_TYPE_LOW_CASE "release")
endif()


message("common-settings - CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
message("common-settings - ANTARES_SRC : ${ANTARES_SRC}")
message("common-settings - ANTARES_BIN : ${ANTARES_BIN}")
message("common-settings - CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("common-settings - UNIT_LIBRARIES_DIR : ${UNIT_LIBRARIES_DIR}")
