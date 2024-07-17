project(infeasible-problem-analysis)

set(SRC_INFEASIBLE_PROBLEM_ANALYSIS
        include/antares/solver/infeasible-problem-analysis/unfeasibility-analysis.h
        include/antares/solver/infeasible-problem-analysis/constraint-slack-analysis.h
        constraint-slack-analysis.cpp
        include/antares/solver/infeasible-problem-analysis/variables-bounds-consistency.h
        variables-bounds-consistency.cpp
        unfeasible-pb-analyzer.cpp
        include/antares/solver/infeasible-problem-analysis/unfeasible-pb-analyzer.h
        include/antares/solver/infeasible-problem-analysis/report.h
        report.cpp
        include/antares/solver/infeasible-problem-analysis/watched-constraints.h
        watched-constraints.cpp
)

add_library(infeasible_problem_analysis ${SRC_INFEASIBLE_PROBLEM_ANALYSIS})
target_link_libraries(infeasible_problem_analysis
        PRIVATE
        ortools::ortools
        Boost::headers
        Antares::logs
)
target_include_directories(infeasible_problem_analysis
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
