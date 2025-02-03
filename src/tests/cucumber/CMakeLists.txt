file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/behave.ini CONTENT
        "[behave.userdata]\
\n\
antares-solver = $<TARGET_FILE:antares-solver>\
\n\
antares-modeler = $<TARGET_FILE:antares-modeler>\
\n\
resources-path = ${CMAKE_SOURCE_DIR}/tests/resources"
        CONDITION $<STREQUAL:$<CONFIG>,${CMAKE_BUILD_TYPE}>)