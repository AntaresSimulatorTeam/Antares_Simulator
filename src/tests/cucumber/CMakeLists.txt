file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/conf.yaml CONTENT
        "antares-solver : $<TARGET_FILE:antares-solver>\
\n\
resources-path : ${CMAKE_SOURCE_DIR}/tests/resources/Antares_Simulator_Tests_NR"
        CONDITION $<STREQUAL:$<CONFIG>,${CMAKE_BUILD_TYPE}>)