add_library(solver_api)
add_library(Antares::solver_api ALIAS solver_api)

set(PUBLIC_HEADERS
        include/antares/api/SimulationResults.h
        include/antares/api/solver.h
)

set(PRIVATE_HEADERS
        private/API.h
        private/SimulationObserver.h
)

target_sources(solver_api
        PRIVATE
        solver.cpp
        API.cpp
        SimulationObserver.cpp
        SimulationResults.cpp
        ${PUBLIC_HEADERS}
        ${PRIVATE_HEADERS}
)

target_include_directories(solver_api
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
)

target_link_libraries(solver_api
        PRIVATE
        Antares::file-tree-study-loader
        Antares::study
        Antares::study-loader
        antares-solver-simulation
        PUBLIC
        Antares::optimization-options
        Antares::lps
)
install(DIRECTORY include/antares
        DESTINATION "include"
)