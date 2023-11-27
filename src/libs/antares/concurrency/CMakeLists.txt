

add_library(concurrency)
add_library(Antares::concurrency ALIAS concurrency)

target_sources(concurrency PRIVATE concurrency.cpp)
target_include_directories(concurrency PUBLIC include)

target_link_libraries(concurrency yuni-static-core)