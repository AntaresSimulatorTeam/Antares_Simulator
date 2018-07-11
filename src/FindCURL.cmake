# Look for the header file.
set (CURL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/ext/libcurl/include)


# Look for the library
set(LIB_CURL_NAME libcurl.a )
set (CURL_LIBRARY ${PROJECT_SOURCE_DIR}/ext/libcurl/lib/.libs/${LIB_CURL_NAME})


mark_as_advanced(CURL_LIBRARY)

if(CURL_INCLUDE_DIR)
	foreach(_curl_version_header curlver.h curl.h)
		if(EXISTS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}")
			file(STRINGS "${CURL_INCLUDE_DIR}/curl/${_curl_version_header}" curl_version_str REGEX "^#define[\t ]+LIBCURL_VERSION[\t ]+\".*\"")
			string(REGEX REPLACE "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]*)\".*" "\\1" CURL_VERSION_STRING "${curl_version_str}")
			unset(curl_version_str)
			break()
		endif()
	endforeach()
endif()



# handle the QUIETLY and REQUIRED arguments and set CURL_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CURL
	REQUIRED_VARS CURL_LIBRARY CURL_INCLUDE_DIR
	VERSION_VAR CURL_VERSION_STRING)



if(CURL_FOUND)
	set(CURL_LIBRARIES ${CURL_LIBRARY})
	set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
endif()
