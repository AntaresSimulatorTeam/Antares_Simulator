add_library(in-memory-study)
add_library(Antares::tests::in-memory-study ALIAS in-memory-study)

target_sources(in-memory-study
        PRIVATE
        in-memory-study.cpp
        include/in-memory-study.h
)

target_link_libraries(in-memory-study
        PUBLIC
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        antares-solver-simulation
        Antares::application
        PRIVATE
        Antares::infoCollection
)

target_include_directories(in-memory-study
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(in-memory-study PROPERTIES FOLDER Unit-tests)

target_include_directories(in-memory-study PUBLIC ${Boost_INCLUDE_DIRS})
