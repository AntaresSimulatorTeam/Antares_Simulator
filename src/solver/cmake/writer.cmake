add_library(zip_writer
            writer/zip_writer.cpp)

# target_include_directories(zip_writer PUBLIC ${CMAKE_SOURCE_DIR}/solver/writer)

target_link_libraries(zip_writer
  PRIVATE
  libantares-core # We only need yuni-static-core, but it can't be included directly. TODO
  libzippp::libzippp)

