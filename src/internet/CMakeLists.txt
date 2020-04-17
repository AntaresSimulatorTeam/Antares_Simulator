
OMESSAGE("License")

add_subdirectory(base64)
add_subdirectory(keys)



if(NOT MSVC)
  add_definitions(-DCURL_STATICLIB)
endif()


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

#TODO : OPENSSL and CURL removed but link still OK ?
#target_link_libraries(libantares-license ${ANTARES_OSSL_LIBS} ${ANTARES_CURL_LIBS} base64)

