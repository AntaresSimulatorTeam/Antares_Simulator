OMESSAGE("Antares Solver")

#TODO : see to add bigobj support

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)


include("cmake/infeasible-problem-analysis.cmake")
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

add_executable(${exec_name}
	memory/mem.h

	misc/options.h
	misc/options.cpp
	misc/process-priority.cpp
	misc/cholesky.h
	misc/cholesky.hxx
	misc/matrix-dp-make.h
	misc/matrix-dp-make.hxx

	misc/system-memory.h
	misc/system-memory.cpp

	mps/mps_define.h
	mps/mps_extern_global.h
	mps/mps_global.c
	${SRCS}
	main.cpp
	application.h application.cpp
)

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

target_link_libraries(${exec_name}
		PRIVATE
			${ANTARES_SOLVER_LIBS}
		)

import_std_libs(${exec_name})
executable_strip(${exec_name})

copy_dependency(sirius_solver ${exec_name})



#TODO : not working inside macro
install(TARGETS "antares-${ANTARES_PRG_VERSION}-solver" EXPORT antares-solver DESTINATION bin)

INSTALL(EXPORT antares-solver
	FILE antares-solverConfig.cmake
	DESTINATION cmake
)

