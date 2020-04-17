set(ANTARES_CURL_INCLUDE  "")
set(ANTARES_CURL_LIBS     "")
set(ANTARES_CURL_LINK     "")
set(ANTARES_OSSL_INCLUDE  "")


if(NOT "${ANTARES_x86_64}" STREQUAL "")
		set(ARCH_TYPE "x86_64")

	else()
		set(ARCH_TYPE "x86")
endif()

if (WIN32)
	if(MSVC)
		set(COMPILER_NAME "vc14")
		set(COMPILER_LIB_INCLUDE "vc")
	else(MSVC)
		set(COMPILER_NAME "gcc4.x")
		set(COMPILER_LIB_INCLUDE "gcc")
	endif(MSVC)

	OMESSAGE("Libcurl support enabled")
	set(ANTARES_CURL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ext/libcurl/builds/libcurl-${COMPILER_NAME}-${ANTARES_INSTALLER_REDIST_ARCH}-release-static-ssl-static-ipv6/include")
	set(ANTARES_CURL_LINK "${CMAKE_CURRENT_SOURCE_DIR}/ext/libcurl/builds/libcurl-${COMPILER_NAME}-${ANTARES_INSTALLER_REDIST_ARCH}-release-static-ssl-static-ipv6/lib")
	
	
	set(ANTARES_OSSL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ext/openssl/${COMPILER_LIB_INCLUDE}-release-${ARCH_TYPE}/include")
	set(ANTARES_OSSL_LINK "${CMAKE_CURRENT_SOURCE_DIR}/ext/openssl/${COMPILER_LIB_INCLUDE}-release-${ARCH_TYPE}/lib")
	if(NOT MINGW)
	    set(ANTARES_CURL_LIBS "${ANTARES_CURL_LINK}/libcurl_a.lib")
		set(ANTARES_OSSL_LIBS "${ANTARES_OSSL_LINK}/libeay32.lib" "${ANTARES_OSSL_LINK}/ssleay32.lib")
	else()
	    set(ANTARES_CURL_LIBS "${ANTARES_CURL_LINK}/libcurl.a")
	    set(ANTARES_OSSL_LIBS "${ANTARES_OSSL_LINK}/libssl.a" "${ANTARES_OSSL_LINK}/libcrypto.a")
	endif()
	
	include_directories("${ANTARES_CURL_INCLUDE}")
	link_directories("${ANTARES_CURL_LINK}")
	OMESSAGE("Libcurl: ${ANTARES_CURL_LINK}")
	
	OMESSAGE("OpenSSL support enabled")
	
else()
	include(FindPackageHandleStandardArgs)

	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

	#libcurl
	find_package(CURL REQUIRED)

	#openssl	
	find_package(OpenSSL REQUIRED)

endif()
