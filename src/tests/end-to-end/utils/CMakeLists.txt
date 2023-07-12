add_library(test_utils
        utils.cpp
        utils.h
        )

target_link_libraries(test_utils
        PRIVATE
        libantares-core
        libantares-solver-simulation
        PUBLIC
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )

target_include_directories(test_utils
        PRIVATE
        ${CMAKE_SOURCE_DIR}/solver
        )

set_target_properties(test_utils PROPERTIES FOLDER Unit-tests/end_to_end)

target_compile_definitions(test_utils PUBLIC "BOOST_TEST_DYN_LINK=1")
target_include_directories(test_utils PUBLIC ${Boost_INCLUDE_DIRS})
