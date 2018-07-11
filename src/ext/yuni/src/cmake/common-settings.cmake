
#
# CMake Stuff
#
#set(CMAKE_VERBOSE_MAKEFILE ON)
# Policy
if(NOT CMAKE_MINOR_VERSION EQUAL 4 OR NOT CMAKE_MAJOR_VERSION EQUAL 2 )
	cmake_policy(SET CMP0004 OLD)
	cmake_policy(SET CMP0003 NEW)
endif()
include(CheckIncludeFile)
include(CheckCXXCompilerFlag)

set_property(GLOBAL PROPERTY USE_FOLDERS true)


if ("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
	set(YUNI_)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
endif()


#
# Macro for automatically detect compiler flag
#
macro(compile_flag  flag varname)
	check_cxx_compiler_flag("${flag}" "YUNI_HAS_CXX_FLAG_${varname}")
	if ("${YUNI_HAS_CXX_FLAG_${varname}}")
		foreach (arg ${ARGN})
			set("CMAKE_CXX_FLAGS_${arg}" "${CMAKE_CXX_FLAGS_${arg}} ${flag}")
		endforeach()
	endif()
	check_c_compiler_flag("${flag}" "YUNI_HAS_C_FLAG_${varname}")
	if ("${YUNI_HAS_C_FLAG_${varname}}")
		foreach (arg ${ARGN})
			set("CMAKE_C_FLAGS_${arg}" "${CMAKE_C_FLAGS_${arg}} ${flag}")
		endforeach()
	endif()
endmacro()


#
# Clang Detection
#
check_cxx_source_compiles("
	#ifndef __clang__
	will never compile !
	#endif
	int main() {return 0;}" YUNI_COMPILER_IS_CLANG)
if ("${YUNI_COMPILER_IS_CLANG}")
	set(CLANG 1)
endif()


#
# Getting the folder where this file is located
#
set(CurrentFolder "${CMAKE_CURRENT_LIST_FILE}")
string(REPLACE "\\common-settings.cmake" "" CurrentFolder "${CurrentFolder}")
string(REPLACE "/common-settings.cmake" "" CurrentFolder "${CurrentFolder}")
# Current Folder : ${CurrentFolder}

#
# Detect Special Instructions Set (SSE, MMX...)
#
include("${CurrentFolder}/DetectInstructionsSets.cmake")


# Common options to all GCC-based compilers
set(YUNI_COMMON_CC_OPTIONS   "")
set(YUNI_COMMON_CC_OPTIONS  "${YUNI_COMMON_CC_OPTIONS} -D_REENTRANT -DXUSE_MTSAFE_API")
set(YUNI_COMMON_CC_OPTIONS  "${YUNI_COMMON_CC_OPTIONS} -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
set(YUNI_COMMON_CXX_OPTIONS  "")

set(COMMON_MSVC_FLAGS "/W3 /MP4 /Zi")
set(COMMON_MSVC_FLAGS "${COMMON_MSVC_FLAGS} /DREENTRANT /D_LARGEFILE_SOURCE /D_LARGEFILE64_SOURCE /D_FILE_OFFSET_BITS=64")
set(COMMON_MSVC_FLAGS "${COMMON_MSVC_FLAGS} /DUNICODE /D_UNICODE")



# Common options for GCC on Unixes (mingw excluded)
# fPIC seems a good choice in most cases
set(YUNI_COMMON_CC_OPTIONS_UNIX   "${YUNI_COMMON_CC_OPTIONS} -fPIC")

include(CheckCXXCompilerFlag)
if(NOT MSVC)
	check_cxx_compiler_flag("-std=gnu++14"   YUNI_HAS_GNU14_SUPPORT)
	check_cxx_compiler_flag("-std=gnu++11"   YUNI_HAS_GNU11_SUPPORT)
	check_cxx_compiler_flag("-std=c++14"     YUNI_HAS_CPP14_SUPPORT)
	check_cxx_compiler_flag("-std=c++11"     YUNI_HAS_CPP11_SUPPORT)
	check_cxx_compiler_flag("-std=c++0x"     YUNI_HAS_GCC_CPP0X_SUPPORT)
	check_cxx_compiler_flag("-stdlib=libc++" YUNI_HAS_LIB_CPP11_SUPPORT) # clang, Apple gcc...

	if (NOT APPLE)
		if (YUNI_HAS_GNU14_SUPPORT)
			set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=gnu++14")
			set(YUNI_CPP_STD "-std=gnu++14" CACHE STRING "c++std" FORCE)
		else()
			if (YUNI_HAS_GNU11_SUPPORT)
				set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=gnu++11")
				set(YUNI_CPP_STD "-std=gnu++11" CACHE STRING "c++std" FORCE)
			else()
				if (YUNI_HAS_GCC_CPP0X_SUPPORT)
					set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=c++0x")
					set(YUNI_CPP_STD "-std=c++0x" CACHE STRING "c++std" FORCE)
				endif()
			endif()
		endif()
	else()
		if (YUNI_HAS_CPP14_SUPPORT)
			set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=c++14")
			set(YUNI_CPP_STD "-std=c++14" CACHE STRING "c++std" FORCE)
		else()
			if (YUNI_HAS_CPP11_SUPPORT)
				set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=c++11")
				set(YUNI_CPP_STD "-std=c++11" CACHE STRING "c++std" FORCE)
			else()
				if (YUNI_HAS_GCC_CPP0X_SUPPORT)
					set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -std=c++0x")
					set(YUNI_CPP_STD "-std=c++0x" CACHE STRING "c++std" FORCE)
				endif()
			endif()
		endif()
	endif()


	if (YUNI_HAS_LIB_CPP11_SUPPORT AND (NOT CLANG OR APPLE))
		# clang seems to not like the option -stdlib, but required on MacOS...
		set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -stdlib=libc++")
	endif()


	# transform some warnings into errors to avoid common mistakes
	check_cxx_compiler_flag("-Werror=switch"   YUNI_HAS_W2E_SWITCH)

	if (YUNI_HAS_W2E_SWITCH)
		set(YUNI_COMMON_CXX_OPTIONS  "${YUNI_COMMON_CXX_OPTIONS} -Werror=switch")
	endif()
endif()

if (APPLE AND CLANG)
	# http://lists.cs.uiuc.edu/pipermail/cfe-dev/2011-January/012999.html
	# Temporary workaround for compiling with Clang on OS X
	set(YUNI_COMMON_CC_OPTIONS_UNIX  "${YUNI_COMMON_CC_OPTIONS_UNIX} -U__STRICT_ANSI__")
endif()




# Unicode on Windows
if(WIN32 OR WIN64)
	add_definitions("-DUNICODE")
	add_definitions("-D_UNICODE")
	add_definitions("-D__UNICODE")
endif()

# Thread safety
add_definitions("-D_REENTRANT -DXUSE_MTSAFE_API")




if(NOT WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE         "${YUNI_COMMON_CC_OPTIONS_UNIX} ${YUNI_COMMON_CXX_OPTIONS}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${YUNI_COMMON_CC_OPTIONS_UNIX} ${YUNI_COMMON_CXX_OPTIONS}")
	set(CMAKE_CXX_FLAGS_DEBUG           "${YUNI_COMMON_CC_OPTIONS_UNIX} ${YUNI_COMMON_CXX_OPTIONS}")

	set(CMAKE_C_FLAGS_RELEASE         "${YUNI_COMMON_CC_OPTIONS_UNIX}")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO  "${YUNI_COMMON_CC_OPTIONS_UNIX}")
	set(CMAKE_C_FLAGS_DEBUG           "${YUNI_COMMON_CC_OPTIONS_UNIX}")
endif()

if(MINGW)
	set(CMAKE_CXX_FLAGS_RELEASE         "${YUNI_COMMON_CC_OPTIONS} ${YUNI_COMMON_CXX_OPTIONS}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${YUNI_COMMON_CC_OPTIONS} ${YUNI_COMMON_CXX_OPTIONS}")
	set(CMAKE_CXX_FLAGS_DEBUG           "${YUNI_COMMON_CC_OPTIONS} ${YUNI_COMMON_CXX_OPTIONS}")

	set(CMAKE_C_FLAGS_RELEASE         "${YUNI_COMMON_CC_OPTIONS}")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO  "${YUNI_COMMON_CC_OPTIONS}")
	set(CMAKE_C_FLAGS_DEBUG           "${YUNI_COMMON_CC_OPTIONS}")
endif()

if(MSVC)
	set(CMAKE_C_FLAGS_DEBUG   "${COMMON_MSVC_FLAGS} /MDd /GR /Ot /Od /EHsc /RTC1")
	set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_MSVC_FLAGS} /MDd /GR /Ot /Od /EHsc /RTC1 /fp:except")

	set(MSVC_RELEASE_FLAGS)

	# O2x: optimization
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /O2")
	# Prefer speed instead of size
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Ot")
	# Omit frame pointer
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Oy")
	# Any suitable inlining
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Ob2")
	# Fiber-safe optimizations
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GT")
	# whole program / requires "Link time code generation"
	#set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GL")
	# No buffer security check
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GS-")
	# SSE2
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /arch:SSE2")
	# Intrinsic functions
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Oi")
	# Multithreaded DLL
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /MD")

	set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_RELEASE_FLAGS}")
	set(CMAKE_C_FLAGS_RELEASE    "${CMAKE_C_FLAGS_RELEASE} ${MSVC_RELEASE_FLAGS}")
endif()

if (NOT MSVC)
	# Optimisation
	compile_flag("-O3"                       O3  RELEASE)
	compile_flag("-fomit-frame-pointer"      FOMIT_FRAME_POINTER  RELEASE RELWITHDEBINFO)
	compile_flag("-fstrict-aliasing"         STRICT_ALIASING      RELEASE RELWITHDEBINFO)
	compile_flag("-msse"                     MSSE                 RELEASE RELWITHDEBINFO)
	compile_flag("-msse2"                    MSSE2                RELEASE RELWITHDEBINFO)
	compile_flag("-fvisibility=hidden"       VISIBILITY_HIDDEN    RELEASE DEBUG RELWITHDEBINFO)
	#compile_flag("-mfpmath=sse"          FPMATH_MSSE          RELEASE RELWITHDEBINFO)
	if (NOT "${CLANG}")
		compile_flag("-fdiagnostics-color=auto"  DIAG_COLORS      RELEASE DEBUG RELWITHDEBINFO)
	endif()

	# link
	#compile_flag("-flto"                 FLTO  RELEASE RELWITHDEBINFO)

	# debugging symbols
	compile_flag("-g"     DEBUG_G     DEBUG RELWITHDEBINFO)
	compile_flag("-g3"    DEBUG_G3    DEBUG RELWITHDEBINFO)
	compile_flag("-gfull" DEBUG_GFULL DEBUG RELWITHDEBINFO)
	compile_flag("-ggdb"  DEBUG_GDB   DEBUG RELWITHDEBINFO)
	compile_flag("-ggdb3" DEBUG_GDB3  DEBUG RELWITHDEBINFO)

	# extra debugging tools
	#compile_flag("-fsanitize=undefined" DEBUG_SANITIZE_UNDEF DEBUG RELWITHDEBINFO)
	#compile_flag("-fsanitize=address"   DEBUG_SANITIZE_ADDR  DEBUG RELWITHDEBINFO)

	# warnings
	compile_flag("-W"                         W   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wall"                      W_ALL   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wextra"                    W_EXTRA   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wunused-parameter"         W_UNUSED_PARAMETER   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wconversion"               W_CONVERSION   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Woverloaded-virtual"       W_OVERLOADED_VIRTUAL   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wundef"                    W_UNDEF   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wfloat-equal"              W_FLOAT_EQUAL   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wmissing-noreturn"         W_MISSING_NORETURN   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wcast-align"               W_CAST_ALIGN   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wuninitialized"            W_UNINITIALIZED   RELEASE DEBUG RELWITHDEBINFO)
	compile_flag("-Wdocumentation"            W_DOCUMENTATION   RELEASE DEBUG RELWITHDEBINFO)
	#compile_flag("-Wold-style-cast"           W_OLD_STYLE_CAST  RELEASE DEBUG RELWITHDEBINFO)
endif()


if (MINGW)
	# mthreads is required on windows with mingw
	compile_flag("-mthreads"   MTHREADS   RELEASE DEBUG RELWITHDEBINFO)
endif()




# NDEBUG
if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG")
	set(CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} /DNDEBUG")
else()
	set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
	set(CMAKE_C_FLAGS_RELEASE           "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
endif()

set(CMAKE_CXX_FLAGS_RELEASE       "${CMAKE_CXX_FLAGS_RELEASE}      ${YUNI_PROFILE_CXX_FLAGS_INSTRUCTIONS_SETS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBUG  "${CMAKE_CXX_FLAGS_RELWITHDEBUG} ${YUNI_PROFILE_CXX_FLAGS_INSTRUCTIONS_SETS}")
set(CMAKE_CXX_FLAGS_DEBUG         "${CMAKE_CXX_FLAGS_DEBUG}        ${YUNI_PROFILE_CXX_FLAGS_INSTRUCTIONS_SETS}")

# Override
if (NOT "${YUNI_CXX_FLAGS_OVERRIDE}" STREQUAL "")
	set(CMAKE_CXX_FLAGS_RELEASE          "${YUNI_CXX_FLAGS_OVERRIDE}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBUG     "${YUNI_CXX_FLAGS_OVERRIDE}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "${YUNI_CXX_FLAGS_OVERRIDE}")
	set(CMAKE_CXX_FLAGS_DEBUG            "${YUNI_CXX_FLAGS_OVERRIDE}")
endif()

if(NOT "${YUNI_CXX_FLAGS_OVERRIDE_ADD_DEBUG}" STREQUAL "")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${YUNI_CXX_FLAGS_OVERRIDE_ADD_DEBUG}")
endif()
if(NOT "${YUNI_CXX_FLAGS_OVERRIDE_ADD_RELEASE}" STREQUAL "")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${YUNI_CXX_FLAGS_OVERRIDE_ADD_RELEASE}")
endif()
if(NOT "${YUNI_CXX_FLAGS_OVERRIDE_ADD_RELWITHDEBINFO}" STREQUAL "")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${YUNI_CXX_FLAGS_OVERRIDE_ADD_RELWITHDEBINFO}")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
endif()



#
# Extra - Mac OS X Bundles
#
if(APPLE)
	set(MACOSX_BUNDLE_COPYRIGHT "Yuni Framework - 2012")

	# The compiler flag -arch must be checked. The compiler might not have apple extensions
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag("arch" YUNI_GCC_HAS_ARCH_FLAG)

	if (YUNI_GCC_HAS_ARCH_FLAG)
		if(YUNI_MACOX_UNIVERSAL_BINARIES)
			YMESSAGE("Enabled universal binaries (custom : ${YUNI_MACOX_UNIVERSAL_BINARIES})")
			set(CMAKE_OSX_ARCHITECTURES "${YUNI_MACOX_UNIVERSAL_BINARIES}") # ppc;i386;ppc64;x86_64
		else()
			YMESSAGE("Enabled universal binaries (i386, x86_64)")
			set(CMAKE_OSX_ARCHITECTURES "i686;x86_64") # ppc;i386;ppc64;x86_64
		endif()
	else()
		YWARNING("Universal binaries disabled. The compiler does not seem to support multiple platform architectures in a single binary)")
	endif()
endif()


