add_library(zip_writer
  writer/i_writer.h
  writer/zip_writer.cpp
  writer/zip_writer.h)

target_link_libraries(zip_writer
  PRIVATE
  libantares-core # We only need yuni-static-core, but it can't be included directly. TODO
  MINIZIP::minizip)

