set(SRC_SOLVER_MAIN_ECONOMY        economy.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY       adequacy.cpp)

#if(MSVC)
#	set_source_files_properties(economy.cpp PROPERTIES COMPILE_FLAGS " /MP")
#	set_source_files_properties(adequacy-draft.cpp PROPERTIES COMPILE_FLAGS " /MP")
#endif(MSVC)



add_library(libantares-solver-main-adequacy  ${SRC_SOLVER_MAIN_ADEQUACY})
target_link_libraries(libantares-solver-main-adequacy
        PRIVATE
        Antares::infoCollection
)

add_library(libantares-solver-main-economy   ${SRC_SOLVER_MAIN_ECONOMY})
target_link_libraries(libantares-solver-main-economy
        PRIVATE
        Antares::infoCollection
)
