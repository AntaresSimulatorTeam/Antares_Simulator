

set(solver_modeler_data_series "${CMAKE_SOURCE_DIR}/solver/modeler/dataSeries")

set(SRC_DATA_SERIES
        include/antares/solver/modeler/dataSeries/dataSeries.h
        include/antares/solver/modeler/dataSeries/dataSeriesRepo.h
        include/antares/solver/modeler/dataSeries/scenarioGroupRepo.h

        scenarioGroupRepo.cpp
)

add_library(data-series-lib ${SRC_DATA_SERIES})
set_target_properties(data-series-lib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(data-series-lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

