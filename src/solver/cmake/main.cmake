

set(SRC_SOLVER_MAIN_ECONOMY        main/economy.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY_DRAFT main/adequacy-draft.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY       main/adequacy.cpp)

#if(MSVC)
#	set_source_files_properties(main/economy.cpp PROPERTIES COMPILE_FLAGS " /MP")
#	set_source_files_properties(main/adequacy-draft.cpp PROPERTIES COMPILE_FLAGS " /MP")
#endif(MSVC)



add_library(libantares-solver-main-adequacy-draft STATIC ${SRC_SOLVER_MAIN_ADEQUACY_DRAFT})
add_library(libantares-solver-main-adequacy STATIC ${SRC_SOLVER_MAIN_ADEQUACY})
add_library(libantares-solver-main-economy  STATIC ${SRC_SOLVER_MAIN_ECONOMY})


add_library(libantares-solver-main-adequacy-draft-swap STATIC ${SRC_SOLVER_MAIN_ADEQUACY_DRAFT})
add_library(libantares-solver-main-adequacy-swap STATIC ${SRC_SOLVER_MAIN_ADEQUACY})
add_library(libantares-solver-main-economy-swap  STATIC ${SRC_SOLVER_MAIN_ECONOMY})


set_target_properties(libantares-solver-main-adequacy-draft-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
set_target_properties(libantares-solver-main-adequacy-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
set_target_properties(libantares-solver-main-economy-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
