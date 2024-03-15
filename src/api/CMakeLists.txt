add_library(solver_api)
add_library(Antares::solver_api ALIAS solver_api)

target_sources(solver_api
        PRIVATE
        solver.cpp
        API.cpp
)

target_include_directories(solver_api
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
)

target_link_libraries(solver_api
        PRIVATE
        Antares::study
        Antares::study-loader
        Antares::file-tree-study-loader
)