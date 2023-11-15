
OMESSAGE("Antares Core library")

add_subdirectory(args)
add_subdirectory(writer)
add_subdirectory(memory)
add_subdirectory(locale)
add_subdirectory(locator)
add_subdirectory(array)
add_subdirectory(correlation)
add_subdirectory(concurrency)

add_subdirectory(logs)
add_subdirectory(jit)
add_subdirectory(inifile)

add_subdirectory(benchmarking)
add_subdirectory(InfoCollection)
add_subdirectory(io)
add_subdirectory(exception)
add_subdirectory(resources)
add_subdirectory(sys)

set(HEADERS
	include/antares/antares/antares.h
	include/antares/antares/constants.h
	include/antares/antares/fatal-error.h
	include/antares/antares/version.h
	include/antares/antares/Enum.hpp
	include/antares/antares/Enum.hxx
)
set(SRC
	${HEADERS}
	constants.cpp
	version.cpp
)
add_subdirectory(date)

add_subdirectory(checks)

add_subdirectory(study)
add_subdirectory(stdcxx)
add_subdirectory(utils)
add_subdirectory(paths)
add_subdirectory(mersenne-twister)
add_subdirectory(series)

add_library(antares-core
	${SRC}
)

#TODO : needed for include yuni : why yuni must be include first ??? cheat : made it public so everybody will have this include (should be done in yuni)
#TODO : needed for include
target_include_directories(antares-core
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
		)

target_link_libraries(antares-core
	PUBLIC
		yuni-static-core
		Antares::logs
        Antares::exception
		Antares::study
		Antares::config
	)

import_std_libs(antares-core)
