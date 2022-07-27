add_library(zip_writer
  # Interface class (public API)
  writer/i_writer.h

  # Generic factory (public API)
  writer/factory.h
  writer/factory.cpp

  # Immediate file writer (private)
  writer/immediate_file_writer.cpp
  writer/immediate_file_writer.h

  # Add entry to zip, using a job queue (private)
  writer/zip_writer.cpp
  writer/zip_writer.h)

target_link_libraries(zip_writer
  PRIVATE
  libantares-core # We only need yuni-static-core, but it can't be included directly. TODO
  MINIZIP::minizip)

