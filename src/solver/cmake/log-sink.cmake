set(ANTARES_LOG_SINK_SRC
    log-sink/log_sink.h
    log-sink/log_sink.cpp)

source_group("log-sink" FILES ${ANTARES_LOG_SINK_SRC})

add_library(libantares-log-sink ${ANTARES_LOG_SINK_SRC})

target_include_directories(libantares-log-sink PUBLIC
                           ${CMAKE_SOURCE_DIR}/solver/log-sink)

target_link_libraries(libantares-log-sink
                      PRIVATE
                      libantares-core
                      PUBLIC
                      ortools::ortools)
