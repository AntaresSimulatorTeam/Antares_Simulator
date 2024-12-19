include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

find_package(antlr4-runtime CONFIG REQUIRED)
add_boost_test(antlr-interface-test
  SRC test_antlr_interface.cpp
  LIBS
  antlr-interface)

target_include_directories(antlr-interface-test
        PRIVATE
        ${ANTLR4_INCLUDE_DIR})
