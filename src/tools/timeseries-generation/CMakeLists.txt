project(timeseries-gen-tool)

add_library(timeseries-gen-tool)

target_sources(timeseries-gen-tool PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/main.cpp>
)

target_link_libraries(series PUBLIC antares-solver-ts-generator)

target_include_directories(series
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
