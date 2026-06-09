set(PROJ linear-problem-data-impl)

set(SRC_DATA_SERIES
        include/antares/optimisation/linear-problem-data-impl/dataSeries.h

        include/antares/optimisation/linear-problem-data-impl/timeSeriesSet.h
        timeSeriesSet.cpp
        timeSeriesSetExceptions.cpp

        include/antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h
        dataSeriesRepo.cpp
        dataSeriesRepoExceptions.cpp

        include/antares/optimisation/linear-problem-data-impl/linearProblemData.h
        linearProblemData.cpp

        include/antares/optimisation/linear-problem-data-impl/Scenario.h
        Scenario.cpp
)

add_library(${PROJ} ${SRC_DATA_SERIES})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        linear-problem-api
        fmt::fmt
        Antares::exception
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

