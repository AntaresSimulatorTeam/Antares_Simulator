
OMESSAGE("License")

add_subdirectory(base64)
add_subdirectory(keys)



if(NOT MSVC)
  add_definitions(-DCURL_STATICLIB)
endif()

include_directories("${ANTARES_OSSL_INCLUDE}")
include_directories("${ANTARES_CURL_INCLUDE}")

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
	target_link_libraries(libantares-license ${ANTARES_CURL_LIBS} ${ANTARES_OSSL_LIBS} base64)

