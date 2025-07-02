add_subdirectory(scenarioBuilderExpression)

add_library(scenario-group-parser)
add_library(Antares::scenario-group-parser ALIAS scenario-group-parser)
target_sources(scenario-group-parser
        PRIVATE
        include/antares/scenarioGroupParser/ScenarioGroupParser.h
        ScenarioGroupParser.cpp
)

target_include_directories(scenario-group-parser
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# TODO: remove this dependency to fmt when possible
find_package(fmt REQUIRED)
target_link_libraries(scenario-group-parser
        PRIVATE
        Antares::scenario-builder-antlr
        fmt::fmt
)

install(DIRECTORY include/antares
        DESTINATION "include"
)