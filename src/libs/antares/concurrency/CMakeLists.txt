

add_library(concurrency)
add_library(Antares::concurrency ALIAS concurrency)

target_sources(concurrency PRIVATE concurrency.cpp)
target_include_directories(concurrency PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

find_package(Threads REQUIRED)
target_link_libraries(concurrency PUBLIC Threads::Threads)

install(DIRECTORY include/antares
        DESTINATION "include"
)
