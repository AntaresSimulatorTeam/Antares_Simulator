

add_library(concurrency)
add_library(Antares::concurrency ALIAS concurrency)

target_sources(concurrency PRIVATE concurrency.cpp)
target_include_directories(concurrency PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(concurrency yuni-static-core)

install(DIRECTORY include/antares
        DESTINATION "include"
)