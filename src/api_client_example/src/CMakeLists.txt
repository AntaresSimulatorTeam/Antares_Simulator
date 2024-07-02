add_library(API_client)

target_sources(API_client
        PRIVATE
        API_client.cpp
        API_client.h
)

find_package(Antares REQUIRED)
target_link_libraries(API_client PUBLIC Antares::solver_api)
target_include_directories(API_client
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)