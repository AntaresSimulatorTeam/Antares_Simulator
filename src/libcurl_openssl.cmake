set(ANTARES_CURL_INCLUDE  "")
set(ANTARES_CURL_LIBS     "")
set(ANTARES_CURL_LINK     "")
set(ANTARES_OSSL_INCLUDE  "")


if (WIN32)
	OMESSAGE("Libcurl support enabled")
	set(ANTARES_CURL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ext/libcurl/include")
	set(ANTARES_CURL_LINK "${CMAKE_CURRENT_SOURCE_DIR}/ext/libcurl/lib/${ANTARES_INSTALLER_REDIST_ARCH}")
	set(ANTARES_CURL_LIBS "${ANTARES_CURL_LINK}/libcurl_a.lib")
	include_directories("${ANTARES_CURL_INCLUDE}")
	link_directories("${ANTARES_CURL_LINK}")
	OMESSAGE("Libcurl: ${ANTARES_FLEXNET_INCLUDE}")
	
	OMESSAGE("OpenSSL support enabled")
	set(ANTARES_OSSL_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/ext/openssl/include")
else()
	include(FindPackageHandleStandardArgs)

	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

	#libcurl
	find_package(CURL 7.47.1)

	if (CURL_FOUND)
		set(ANTARES_CURL_INCLUDE "${CURL_INCLUDE_DIRS}")
		set(ANTARES_CURL_LINK "${CURL_LIBRARIES}")
		set(ANTARES_CURL_LIBS "${ANTARES_CURL_LINK}")
	else()	
		OERROR("libcurl-dev is required")
	endif()

	#openssl
	if(NOT OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "/") # default value
	endif()
	
	find_package(OpenSSL 1.1.0)
	
	if (OPENSSL_FOUND)
		set(ANTARES_OSSL_INCLUDE "${OPENSSL_INCLUDE_DIR}")		
		set(ANTARES_OSSL_LINK "${OPENSSL_LIBRARIES}")
		set(ANTARES_OSSL_LIBS "${ANTARES_OSSL_LINK}")		
	else()
		OERROR("libssl-dev is required")
	endif()

endif()
