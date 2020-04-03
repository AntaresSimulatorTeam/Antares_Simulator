# To be used as follows :
# + Go to Antares root directory
# + Execute cmake command :
#	> cmake -DBOOST_PATH=D:/Users/pierregui/Desktop/Tests-U/boost_1_72_0 -A x64 -S tests -B tests/build

cmake_minimum_required(VERSION 3.10)

project(test_antares VERSION 1.0)

include(../root.cmake)

# Common definitions and setings
include(cmake/common-settings.cmake)

message("Tests top - ANTARES_SRC : ${ANTARES_ROOT}")


message("Root CMakeLists - BOOST_PATH : ${BOOST_PATH}")
if(NOT DEFINED BOOST_PATH OR NOT EXISTS ${BOOST_PATH})
	message(FATAL_ERROR "Root CMakeLists - Path to Boost library does not exist. Use -DBOOST_PATH option.")
endif()


set(YUNI_STATIC_CORE_LIB_DIR "${ANTARES_SRC}/ext/yuni/src/build/${BUILD_TYPE_LOW_CASE}/lib/${BUILD_TYPE_UP_CASE}")
find_library(YUNI_STATIC_CORE_LIB NAME yuni-static-core PATHS "${YUNI_STATIC_CORE_LIB_DIR}")
if(NOT YUNI_STATIC_CORE_LIB)
	message(FATAL_ERROR "Root CMakeLists - yuni-static-core does not exist")
else()
	message("Root CMakeLists - YUNI_STATIC_CORE_LIB : ${YUNI_STATIC_CORE_LIB} - found")
endif()


# General actions
add_definitions(-DTESTING)


add_subdirectory(src)
