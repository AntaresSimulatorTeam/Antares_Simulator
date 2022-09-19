add_library(result_writer
  # Interface class (public API)
  writer/i_writer.h

  # Helper class
  writer/ensure_queue_started.h
  writer/ensure_queue_started.cpp

  # Generic factory (public API)
  writer/writer_factory.h
  writer/writer_factory.cpp

  # Immediate file writer (private)
  writer/immediate_file_writer.h
  writer/immediate_file_writer.cpp

  # Add entry to zip, using a job queue (private)
  writer/zip_writer.cpp
  writer/zip_writer.h)

target_include_directories(result_writer PUBLIC
                          ${CMAKE_SOURCE_DIR}/solver/writer)

target_link_libraries(result_writer
                      PRIVATE
                      libantares-core
                      MINIZIP::minizip-ng)


