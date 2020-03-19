

include_directories("${ANTARES_SRC}/libs/antares/array")
add_executable(tests-on-Matrix array/tests-matrix.cpp)
target_link_libraries(tests-on-Matrix ${ANTARES_CORE_LIB} ${YUNI_STATIC_CORE_LIB})