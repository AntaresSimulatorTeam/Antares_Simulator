
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PNE)

OMESSAGE("Antares Solver")





if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
else(MSVC)
	link_libraries("m")
endif(MSVC)




include_directories("${CMAKE_CURRENT_SOURCE_DIR}/mps")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/branchAndBound")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/simplexe/lu")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/presolve")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/simplexe")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/pne")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/branchAndBound")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/allocateur")
include_directories("${CMAKE_SOURCE_DIR}/ext/Sirius_Solver/pointInterieur")

include("cmake/solver.cmake")
include("cmake/hydro.cmake")


include("cmake/main.cmake")
include("cmake/variable.cmake")
include("cmake/randomness.cmake")
include("cmake/simulation.cmake")
include("cmake/ts-generator.cmake")
include("cmake/constraints-builder.cmake")
# Pour trouver les fonctions dans libs/*
include_directories("../libs")



#
# Enable the whole capabilities of the solver
#
# As some old pieces of the solver use static variables for nothing
# we use a define to remove those useful parts (for Antares)
# Uncomment the define to re-enable then
#
#ADD_DEFINITIONS("-DSOLVER_DECLARE_ALL_SYMBOLS")




#
# Resource file for Windows
#
if(WIN32)
	file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/win32/solver.o")
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





macro(solver_app name swap)
	if(${swap})
		OMESSAGE("  :: application ${name} (with swap support)")
	else()
		OMESSAGE("  :: application ${name}")
	endif()
	set(exec_name "antares-${ANTARES_PRG_VERSION}-solver")
	if(${swap})
		set(exec_name "${exec_name}-swap")
	endif()

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
		main.h main.hxx main.cpp
	)

	set(ANTARES_SOLVER_LIBS
		libantares-core-calendar
		libantares-license
		yuni-static-core
		yuni-static-uuid
		${CMAKE_THREADS_LIBS_INIT}
	)

	if(WIN32)
		set(ANTARES_SOLVER_LIBS  ${ANTARES_SOLVER_LIBS} wsock32)
	endif()

	if(${swap})
		set(ANTARES_SOLVER_LIBS  ${ANTARES_SOLVER_LIBS}
			libantares-solver-main-economy-swap
			libantares-solver-main-adequacy-draft-swap
			libantares-solver-main-adequacy-swap
			libantares-solver-hydro-swap
			libantares-solver-aleatoire-swap
			libantares-solver-variable-swap
			libantares-solver-simulation-swap
			libantares-solver-ts-generator-swap
#			libantares-rtesolver-swap
			libmodel_antares-swap
			libsolver_antares-swap
			libantares-core-swap)
		set_target_properties(${exec_name} PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
	else()
		set(ANTARES_SOLVER_LIBS  ${ANTARES_SOLVER_LIBS}
			libantares-solver-main-economy
			libantares-solver-main-adequacy-draft
			libantares-solver-main-adequacy
			libantares-solver-hydro
			libantares-solver-aleatoire
			libantares-solver-variable
			libantares-solver-simulation
			libantares-solver-ts-generator
#			libantares-rtesolver
			libmodel_antares 
			libsolver_antares			
			libantares-core)
	endif()

	if(NOT MSVC)
		#-Wl,--start-group ${ANTARES_SOLVER_LIBS} -Wl,--end-group)
		target_link_libraries(${exec_name} ${ANTARES_SOLVER_LIBS} uuid)
		target_link_libraries(${exec_name}  z uuid dl ssh2 idn idn2 rt)
	else()
		target_link_libraries(${exec_name} ${ANTARES_SOLVER_LIBS})
		target_link_libraries(${exec_name} ole32 rpcrt4) # uuid
	endif()

	import_std_libs(${exec_name})
	executable_strip(${exec_name})

endmacro()




solver_app("solver"      false)
solver_app("solver-swap" true)

