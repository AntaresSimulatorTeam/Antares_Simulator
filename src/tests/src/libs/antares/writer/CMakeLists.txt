include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

# ---------------
# Zip writer
# ---------------
add_boost_test(test-writer
  SRC test_writer.cpp
  LIBS
  Antares::result_writer
  test_utils_unit
  MINIZIP::minizip)

# ------------------------------------------
# Target : test-write-parquet-standalone
# ------------------------------------------
# Testing parquet table writer, in standalone mode (not nested in production code)
find_package(Parquet REQUIRED)
find_package(Arrow REQUIRED)

add_executable(test-write-parquet-standalone test-write-parquet-standalone.cpp)

target_link_libraries(test-write-parquet-standalone
  PRIVATE
  Boost::unit_test_framework
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Arrow::arrow_static,Arrow::arrow_shared>"
  "$<IF:$<BOOL:${ARROW_BUILD_STATIC}>,Parquet::parquet_static,Parquet::parquet_shared>"
)

add_test(NAME test-write-parquet-standalone COMMAND test-write-parquet-standalone
    --report_format=XML
    --report_level=detailed
    --report_sink=report_test-write-parquet-standalone.xml
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Adding labels allows ctest filter what tests to run
set_property(TEST test-write-parquet-standalone PROPERTY LABELS unit)

# Give the IDE some directions to display tests in a "Unit-tests" folder
set_target_properties(test-write-parquet-standalone PROPERTIES FOLDER Unit-tests)

# copy_project_shared_lib(parquet_writer test-write-parquet-standalone)

