
set(ANTARES_ERROR_HAS_OCCURED   false)


macro(OMESSAGE msg)
	if(UNIX)
		message(STATUS "[1;30m{antares}[0m  ${msg}")
	else()
		message(STATUS "{antares}  ${msg}")
	endif()

endmacro()


macro(OMESSAGE_BOLD msg)
	if(UNIX)
		message(STATUS "[1;30m{antares}[0m  [1;37m[1m${msg}[0m")
	else()
		message(STATUS "{antares}  ${msg}")
	endif()
endmacro()


macro(OMESSAGE_TITLE msg1 msg2)
	if(UNIX)
		message(STATUS "[1;30m{antares}[0m  [1;32m${msg1}[0m[1;37m[1m${msg2}[0m")
	else()
		message(STATUS "{antares}  ${msg1}${msg2}")
	endif()
endmacro()


macro(OMESSAGE_MODULE msg)
	OMESSAGE_TITLE("[module] " "${msg}")
endmacro()


macro(OWARNING msg)
	if(UNIX)
		message(STATUS "[1;33m{antares}  [warning][0m ${msg}")
	else()
		message(STATUS "{antares}  [WARNING] ${msg}")
	endif()
endmacro()


macro(OERROR msg)
	if(UNIX)
		message(STATUS "[1;31m{antares}  [error][0m ${msg}")
	else()
		message(STATUS "{antares}  [ERROR] ${msg}")
	endif()
	set(ANTARES_ERROR_HAS_OCCURED  true)
endmacro()


macro(OFATAL msg)
	if(UNIX)
		message(FATAL_ERROR "[1;31m{antares}  [error][0m ${msg}")
	else()
		message(FATAL_ERROR "{antares}  [ERROR] ${msg}")
	endif()

	set(ANTARES_ERROR_HAS_OCCURED  true)
endmacro()


