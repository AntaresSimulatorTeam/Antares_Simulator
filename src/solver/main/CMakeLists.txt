set(SRC_SOLVER_MAIN_ECONOMY        economy.cpp)
set(SRC_SOLVER_MAIN_ADEQUACY       adequacy.cpp)

#if(MSVC)
#	set_source_files_properties(economy.cpp PROPERTIES COMPILE_FLAGS " /MP")
#	set_source_files_properties(adequacy-draft.cpp PROPERTIES COMPILE_FLAGS " /MP")
#endif(MSVC)



add_library(antares-solver-main-adequacy  ${SRC_SOLVER_MAIN_ADEQUACY})
target_link_libraries(antares-solver-main-adequacy
        PRIVATE
        Antares::infoCollection
        Antares::application
        antares-solver-simulation
)

add_library(antares-solver-main-economy   ${SRC_SOLVER_MAIN_ECONOMY})
target_link_libraries(antares-solver-main-economy
        PRIVATE
        Antares::infoCollection
        Antares::application
        antares-solver-simulation
)
