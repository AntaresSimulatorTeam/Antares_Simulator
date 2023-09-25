
#
# CMake Generator / Compiler
#
YMESSAGE_TITLE("Generator : "  "${CMAKE_GENERATOR}")
if(MSVC)
	if (WIN64)
		YMESSAGE("Compiler: Visual Studio (64bits)")
	else()
		YMESSAGE("Compiler: Visual Studio (32bits)")
	endif()
endif()

if(MINGW)
	if (WIN64)
		YMESSAGE("Compiler: MinGW (64bits)")
	else()
		YMESSAGE("Compiler: MinGW (32bits)")
	endif()
endif()

if(XCODE)
	YMESSAGE("Compiler: XCode")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
	if(NOT GNUCXX_VERSION)
		# -dumpversion might be used
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} "--version" OUTPUT_VARIABLE GNUCXX_VERSION_N ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
		string(REGEX REPLACE "Copyright.*" "" GNUCXX_VERSION_N "${GNUCXX_VERSION_N}")
		string(STRIP "${GNUCXX_VERSION_N}" GNUCXX_VERSION_N)
		set(GNUCXX_VERSION "${GNUCXX_VERSION_N}" CACHE INTERNAL "Version of the Gnu Gxx compiler")
	endif()
	YMESSAGE("g++ Variant : ${GNUCXX_VERSION}")
endif()




#
# Information about the current operating system
#
if(APPLE)
	execute_process(COMMAND sw_vers -productName OUTPUT_VARIABLE SW_PN OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sw_vers -productVersion OUTPUT_VARIABLE SW_PV OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sw_vers -buildVersion OUTPUT_VARIABLE SW_BV OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sysctl -n hw.packages OUTPUT_VARIABLE HI_CPU_COUNT OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sysctl -n machdep.cpu.core_count OUTPUT_VARIABLE HI_CPU_CORE_COUNT
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	# SSE
	execute_process(COMMAND sysctl -n hw.optional.sse OUTPUT_VARIABLE CPU_SSE1 OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sysctl -n hw.optional.sse2 OUTPUT_VARIABLE CPU_SSE2 OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND sysctl -n hw.optional.sse3 OUTPUT_VARIABLE CPU_SSE3 OUTPUT_STRIP_TRAILING_WHITESPACE)
	# 64Bits ?
	execute_process(COMMAND sysctl -n hw.cpu64bit_capable OUTPUT_VARIABLE HI_CPU_i64
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	# Display
	YMESSAGE("System: ${SW_PN} ${SW_PV} ${SW_BV} - ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
	YMESSAGE("   ${HI_CPU_COUNT} CPUs (${HI_CPU_CORE_COUNT}-Core), SSE1:${CPU_SSE1}, SSE2:${CPU_SSE2}, "
		"SSE3:${CPU_SSE3}, x86_64:${HI_CPU_i64}")
else()
	YMESSAGE("System: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
endif()



