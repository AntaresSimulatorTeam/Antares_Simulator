
if (CMAKE_COMPILER_IS_GNUCXX)
	# Check for valid version of GCC
	check_cxx_source_compiles("
		#ifdef __GNUC__
		# define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
		# if GCC_VERSION < 40500
		#	error \"GCC >=4.5 required\"
		# endif
		#endif
		int main() {return 0;}" OPTIMATE_HAS_GCC_GREATER_THAN_45)

	if (NOT OPTIMATE_HAS_GCC_GREATER_THAN_45)
		message(FATAL_ERROR "\n\nGCC >= 4.5 is required\n")
	endif()

else()
	if(MSVC)
		if (MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR CMAKE_COMPILER_2005 OR MSVC90)
			message(FATAL_ERROR "\n\nVisual Studio >=10 is required\n")
		endif()
	endif()
endif()

