

add_library(concurrency)
add_library(Antares::concurrency ALIAS concurrency)

target_sources(concurrency PRIVATE concurrency.cpp)
target_include_directories(concurrency PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(concurrency yuni-static-core)