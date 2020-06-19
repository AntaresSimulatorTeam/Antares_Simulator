
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
	getstarttime.cpp
	getstarttime.h	
)

#TODO : needed for include yuni : why yuni must be include first ???
target_include_directories(libantares-license PRIVATE ../ext/yuni/src)

#TODO : needed for include antares core
target_include_directories(libantares-license PRIVATE ../libs)

target_link_libraries(libantares-license PRIVATE yuni-static-core)

target_include_directories(libantares-license PUBLIC ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})


if(WIN32 OR WIN64)
#VCPKG define CURL::libcurl / need to add ${CURL_LIBRARIES} in case of BUILD_DEPS option ON
target_link_libraries(libantares-license PUBLIC CURL::libcurl  ${OPENSSL_LIBRARIES} base64 ${CURL_LIBRARIES})
else()
#CMake on linux define CURL_LIBRARIES
target_link_libraries(libantares-license PUBLIC ${CURL_LIBRARIES}  ${OPENSSL_LIBRARIES} base64 )
endif()

