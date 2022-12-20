OMESSAGE("Antares Solver")

#TODO : see to add bigobj support

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)


include("cmake/infeasible-problem-analysis.cmake")
include("cmake/writer.cmake")
include("cmake/solver.cmake")
include("cmake/hydro.cmake")
include("cmake/main.cmake")
include("cmake/variable.cmake")
include("cmake/randomness.cmake")
include("cmake/simulation.cmake")
include("cmake/ts-generator.cmake")
include("cmake/constraints-builder.cmake")

#
# Resource file for Windows
#
if(WIN32)
	file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.o")
	FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/win32/")
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.rc.cmake"
		"${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.rc")

	if(MINGW)
		# resource compilation for mingw
		add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/win32/solver.o"
			COMMAND windres.exe "-I${CMAKE_CURRENT_SOURCE_DIR}"
			"-i${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.rc"
			-o "${CMAKE_CURRENT_BINARY_DIR}/win32/solver.o")
		set(SRCS ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/win32/solver.o")
	else()
		set(SRCS ${SRCS} "win32/solver.rc")
	endif()
endif()



OMESSAGE("  :: application solver")

set(exec_name "antares-${ANTARES_PRG_VERSION}-solver")

add_library(solver-lib
		memory/mem.h

		misc/headers/options.h
		misc/options.cpp
		misc/process-priority.cpp
		misc/headers/cholesky.h
		misc/cholesky.hxx
		misc/headers/matrix-dp-make.h
		misc/matrix-dp-make.hxx

		misc/headers/system-memory.h
		misc/system-memory.cpp

		mps/mps_define.h
		mps/mps_extern_global.h
		mps/mps_global.c
		${SRCS}
		application/application.h application/application.cpp

		signal-handling/common.h
		signal-handling/public.h
		signal-handling/common.cpp
		signal-handling/linux.cpp
		signal-handling/windows.cpp
		)


add_executable(antares-solver
	main.cpp
    )

set_target_properties(antares-solver PROPERTIES OUTPUT_NAME ${exec_name})

set(ANTARES_SOLVER_LIBS		
	libantares-core-calendar
	yuni-static-uuid
	yuni-static-core		
	${CMAKE_THREADS_LIBS_INIT}
)

set(ANTARES_SOLVER_LIBS  ${ANTARES_SOLVER_LIBS}
	libantares-solver-main-economy
	libantares-solver-main-adequacy-draft
	libantares-solver-main-adequacy
	libantares-solver-hydro
	libantares-solver-aleatoire
	libantares-solver-variable
	libantares-solver-simulation
	libantares-solver-ts-generator
	libmodel_antares
	libantares-core)

target_link_libraries(solver-lib
		PUBLIC
		${ANTARES_SOLVER_LIBS}
		)

target_link_libraries(antares-solver
		PRIVATE
		solver-lib
		)

import_std_libs(antares-solver)
executable_strip(antares-solver)

copy_dependency(sirius_solver antares-solver)

include(GNUInstallDirs)

set(public_headers
		application/application.h
		misc/headers/cholesky.h
		misc/headers/matrix-dp-make.h
		misc/headers/options.h
		misc/headers/system-memory.h
		)
#set_target_properties(solver-lib PROPERTIES PUBLIC_HEADER "${public_headers}")
install(TARGETS solver-lib ${ANTARES_SOLVER_LIBS}
		result_writer infeasible_problem_analysis libantares-solver-variable-info
		minizip
		EXPORT "solver-libTargets"
#		# these get default values from GNUInstallDirs, no need to set them
#		#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
#		#LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
#		#ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
#		# except for public headers, as we want them to be inside a library folder
#		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/solver-lib # include/SomeLibrary
#		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
		)

foreach(header ${public_headers})
	message(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
	message(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	FILE(RELATIVE_PATH header_file_path ${CMAKE_CURRENT_SOURCE_DIR}/solver ${CMAKE_CURRENT_SOURCE_DIR}/${header})
	message(header_file_path ${header_file_path})
	get_filename_component(header_directory_path "${header_file_path}" DIRECTORY)
	message("header_directory_path: " ${header_directory_path})
	install(
			FILES ${header}
			DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/solver-lib/${header_directory_path}"
	)
endforeach()


#TODO : not working inside macro
install(TARGETS antares-solver EXPORT antares-solver DESTINATION bin)

INSTALL(EXPORT antares-solver
	FILE antares-solverConfig.cmake
	DESTINATION cmake
)


#########

# generate and install export file
install(EXPORT "solver-libTargets"
		FILE "solver-libTargets.cmake"
		NAMESPACE ${namespace}::
		DESTINATION cmake
		)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/solver-libConfigVersion.cmake"
		VERSION "${version}"
		COMPATIBILITY AnyNewerVersion
)
# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/solver-libConfig.cmake"
		INSTALL_DESTINATION cmake
		)
# install config files
install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/solver-libConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/solver-libConfigVersion.cmake"
		DESTINATION cmake
		)
# generate the export targets for the build tree
export(EXPORT "solver-libTargets"
		FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/solver-libTargets.cmake"
		NAMESPACE ${namespace}::
		)
