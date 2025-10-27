# Solver API
# This API relies on a callback mechanism ("SimulationObserver")
# Whenever a problem is constructed this callback is called
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

find_package(Boost REQUIRED COMPONENTS iostreams)
target_link_libraries(solver_api
        PRIVATE
        Antares::file-tree-study-loader
        Antares::study
        Antares::study-loader
        antares-solver-simulation
        Boost::iostreams
        PUBLIC
        Antares::optimization-options
        Antares::lps)

# Single problem library
# This API allows the developper to build problems on-demand
add_library(single_problem_api)

target_sources(single_problem_api
	PRIVATE
	singleProblemGetter.cpp
	singleProblemGetterImpl.cpp
	include/antares/api/singleProblemGetter.h)

target_include_directories(single_problem_api
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>)

target_link_libraries(single_problem_api
        PRIVATE
        antares-solver-simulation
        Antares::optimization-options
	model_antares
	Antares::hydro
        PUBLIC
        Antares::study
        Antares::lps)

# TODO move
add_executable(api_test main.cpp)
target_link_libraries(api_test
	single_problem_api
	Antares::file-tree-study-loader)

install(DIRECTORY include/antares
        DESTINATION "include")
