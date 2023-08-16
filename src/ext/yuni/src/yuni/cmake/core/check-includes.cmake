

# stdint.h
check_include_file(stdint.h YUNI_HAS_STDINT_H)

# stdio.h
check_include_file(stdio.h YUNI_HAS_STDIO_H)

# string.h
check_include_file(string.h YUNI_HAS_STRING_H)

# time.h
check_include_file(time.h YUNI_HAS_TIME_H)

# assert.h
check_include_file(assert.h YUNI_HAS_ASSERT_H)

# cassert
check_include_file_cxx(cassert YUNI_HAS_CASSERT)

# errno.h
check_include_file(errno.h YUNI_HAS_ERRNO_H)

# cstddef
check_include_file_cxx(cstddef YUNI_HAS_CSTDDEF)



# climits
check_include_file_cxx(climits YUNI_HAS_CLIMITS)

# vector
check_include_file_cxx(vector YUNI_HAS_VECTOR)

# list
check_include_file_cxx(list YUNI_HAS_LIST)

# map
check_include_file_cxx(map YUNI_HAS_MAP)

# algorithm
check_include_file_cxx(algorithm YUNI_HAS_ALGORITHM)

# iostream
check_include_file_cxx(iostream YUNI_HAS_IOSTREAM)

# cassert
check_include_file_cxx(cassert YUNI_HAS_CASSERT)

# dirent.h
check_include_file(dirent.h YUNI_HAS_DIRENT_H)

# stdlib.h
check_include_file(stdlib.h YUNI_HAS_STDLIB_H)

# unistd.h
check_include_file(unistd.h YUNI_HAS_UNISTD_H)

# fcntl.h
check_include_file(fcntl.h YUNI_HAS_FCNTL_H)

# cstdlib
check_include_file(cstdlib YUNI_HAS_CSTDLIB)

# stdarg
check_include_file(stdarg.h YUNI_HAS_STDARG_H)
if(NOT APPLE AND NOT WIN32 AND NOT WIN64)
	# sys/sendfile.h
	check_include_file_cxx("sys/sendfile.h" YUNI_HAS_SYS_SENDFILE_H)
endif()

if(NOT MSVC)
	# sys/types.h
	check_include_file("sys/types.h" YUNI_HAS_SYS_TYPES_H)
endif(NOT MSVC)

if(WIN32 OR WIN64)
	# cstddef
	check_include_file_cxx(winsock2.h YUNI_HAS_WINSOCK_2)
	# ShellApi.h
	check_include_file_cxx(shellapi.h YUNI_HAS_SHELLAPI_H)
endif(WIN32 OR WIN64)


