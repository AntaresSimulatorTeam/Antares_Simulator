
set(SRC_DATA_SERIES
        include/antares/solver/modeler/dataSeries/dataSeries.h

        include/antares/solver/modeler/dataSeries/timeSeriesSet.h
        timeSeriesSet.cpp
        timeSeriesSetExceptions.cpp

        include/antares/solver/modeler/dataSeries/dataSeriesRepo.h
        dataSeriesRepo.cpp
	dataSeriesRepoExceptions.cpp

        include/antares/solver/modeler/dataSeries/scenarioGroupRepo.h
        scenarioGroupRepo.cpp
        scenarioGroupRepoExceptions.cpp

        include/antares/solver/modeler/dataSeries/linearProblemData.h
        linearProblemData.cpp
)

add_library(data-series-lib ${SRC_DATA_SERIES})
set_target_properties(data-series-lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(data-series-lib
        PUBLIC
        modeler_api
)

target_include_directories(data-series-lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

