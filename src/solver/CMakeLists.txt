OMESSAGE("Antares Solver")

#TODO : see to add bigobj support

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)


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
		yuni-static-uuid
		yuni-static-core		
		${CMAKE_THREADS_LIBS_INIT}
	)

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
			libmodel_antares 
			libsolver_antares			
			libantares-core)
	endif()

	target_link_libraries(${exec_name}
			PRIVATE
				${ANTARES_SOLVER_LIBS}
			)
	
	import_std_libs(${exec_name})
	executable_strip(${exec_name})

endmacro()

solver_app("solver"      false)
solver_app("solver-swap" true)

#TODO : not working inside macro
install(TARGETS "antares-${ANTARES_PRG_VERSION}-solver" DESTINATION bin)
install(TARGETS "antares-${ANTARES_PRG_VERSION}-solver-swap" DESTINATION bin)

