
OMESSAGE("License")

add_subdirectory(base64)
add_subdirectory(keys)

add_library(libantares-license
	license.h
	hello_web.cpp
	host-properties.cpp
	limits.h
	limits.cpp
	licensetimer.cpp
	licensetimer.h
)

#TODO : needed for include yuni : why yuni must be include first ???
#TODO : needed for include antares core
target_include_directories(libantares-license
		PRIVATE
			${CMAKE_SOURCE_DIR}/ext/yuni/src
			${CMAKE_SOURCE_DIR}/libs
		)

target_link_libraries(libantares-license
		PRIVATE
			yuni-static-core
		PUBLIC
			base64
		)

