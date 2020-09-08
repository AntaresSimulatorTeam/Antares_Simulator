

set(SRC_SOLVER_MAIN_ECONOMY        main/economy.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY_DRAFT main/adequacy-draft.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY       main/adequacy.cpp)

#if(MSVC)
#	set_source_files_properties(main/economy.cpp PROPERTIES COMPILE_FLAGS " /MP")
#	set_source_files_properties(main/adequacy-draft.cpp PROPERTIES COMPILE_FLAGS " /MP")
#endif(MSVC)



add_library(libantares-solver-main-adequacy-draft  ${SRC_SOLVER_MAIN_ADEQUACY_DRAFT})
target_link_libraries(libantares-solver-main-adequacy-draft PRIVATE libantares-core)

add_library(libantares-solver-main-adequacy  ${SRC_SOLVER_MAIN_ADEQUACY})
target_link_libraries(libantares-solver-main-adequacy PRIVATE libantares-core)

add_library(libantares-solver-main-economy   ${SRC_SOLVER_MAIN_ECONOMY})
target_link_libraries(libantares-solver-main-economy PRIVATE libantares-core)

if(BUILD_SWAP)
add_library(libantares-solver-main-adequacy-draft-swap  ${SRC_SOLVER_MAIN_ADEQUACY_DRAFT})
target_link_libraries(libantares-solver-main-adequacy-draft-swap PRIVATE libantares-core-swap)

add_library(libantares-solver-main-adequacy-swap  ${SRC_SOLVER_MAIN_ADEQUACY})
target_link_libraries(libantares-solver-main-adequacy-swap PRIVATE libantares-core-swap)

add_library(libantares-solver-main-economy-swap   ${SRC_SOLVER_MAIN_ECONOMY})
target_link_libraries(libantares-solver-main-economy-swap PRIVATE libantares-core-swap)


set_target_properties(libantares-solver-main-adequacy-draft-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
set_target_properties(libantares-solver-main-adequacy-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
set_target_properties(libantares-solver-main-economy-swap
	PROPERTIES COMPILE_FLAGS " -DANTARES_SWAP_SUPPORT=1")
endif()