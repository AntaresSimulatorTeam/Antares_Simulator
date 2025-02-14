set(SRC_DATA_SERIES
        include/antares/optimisation/linear-problem-data-impl/dataSeries.h

        include/antares/optimisation/linear-problem-data-impl/timeSeriesSet.h
        timeSeriesSet.cpp
        timeSeriesSetExceptions.cpp

        include/antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h
        dataSeriesRepo.cpp
		dataSeriesRepoExceptions.cpp

        include/antares/optimisation/linear-problem-data-impl/scenarioGroupRepo.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp

        include/antares/optimisation/linear-problem-data-impl/linearProblemData.h
        linearProblemData.cpp
)

add_library(linear-problem-data-impl ${SRC_DATA_SERIES})
set_target_properties(linear-problem-data-impl PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(linear-problem-data-impl
        PUBLIC
        linear-problem-api
)

target_include_directories(linear-problem-data-impl
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

