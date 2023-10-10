
OMESSAGE("Antares Core library")

add_subdirectory(args)
add_subdirectory(writer)
add_subdirectory(memory)

add_subdirectory(object)

add_subdirectory(array)
add_subdirectory(correlation)


add_subdirectory(logs)
add_subdirectory(jit)
add_subdirectory(inifile)

add_subdirectory(benchmarking)
add_subdirectory(InfoCollection)
add_subdirectory(io)
add_subdirectory(exception)

add_subdirectory(sys)

set(SRC
    config.h
    constants.h
	constants.cpp
    xpansion.h
	# Misc

	locale.h
	locale.cpp

	fatal-error.h

	# resource
	resources/resources.h
	resources/resources.cpp

	solver.h
	solver.cpp

	# version
	version.h
	version.cpp

    # Enum
    # TODO : should be in a target_include_directories : but need to remove all .h from all SRC
    Enum.hpp
    Enum.hxx
)
add_subdirectory(date)

add_subdirectory(checks)

add_subdirectory(study)
add_subdirectory(stdcxx)
add_subdirectory(utils)
add_subdirectory(paths)
add_subdirectory(mersenne-twister)

add_library(antares-core
	antares.h
	${SRC}
)

#TODO : needed for include yuni : why yuni must be include first ??? cheat : made it public so everybody will have this include (should be done in yuni)
#TODO : needed for include
target_include_directories(antares-core
		PRIVATE
			${CMAKE_SOURCE_DIR}/libs
		)

target_link_libraries(antares-core
	PUBLIC
		yuni-static-core
		Antares::logs
        Antares::exception
	)

import_std_libs(antares-core)
