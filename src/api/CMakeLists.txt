add_library(solver_api)
add_library(Antares::solver_api ALIAS solver_api)

target_sources(solver_api
    PRIVATE
        solver.cpp
)

target_include_directories(solver_api
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)