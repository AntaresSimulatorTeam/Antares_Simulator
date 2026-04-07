include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-intermediate
  SRC test_intermediate.cpp dummyVariable.h
  LIBS antares-solver-variable)

add_boost_test(test-surveyresults
  SRC test_surveyresults.cpp
  LIBS
  antares-solver-variable
  result_writer)

add_boost_test(test-residual-load
        SRC test_residual_load.cpp
        LIBS
        antares-solver-variable)

add_boost_test(test-dynamic-aggregation
    SRC test_dynamic.cpp
    LIBS
    antares-solver-variable
    in-memory-study)

add_boost_test(test-setofareas-reports
    SRC test_setofareas_reports.cpp
    LIBS
    antares-solver-variable
    result_writer)

# The test exercises deep template call chains (SurveyReportBuilder) combined with
# large-framed helpers (SurveyResults ~5KB, IntermediateValues ~3KB) on top of
# Boost.Test's own SEH/exception-handling overhead, which together can exceed the
# default 1 MB Windows stack.  Reserve 8 MB to give the test enough headroom.
if(MSVC)
    # 8 MB = 8 * 1024 * 1024 = 8388608 bytes
    target_link_options(test-setofareas-reports PRIVATE /STACK:8388608)
endif()
