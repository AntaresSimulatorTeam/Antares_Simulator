
set(local_target "shave-peaks-by-remix-storage-gen")

add_library(${local_target})

target_sources(${local_target}
        PRIVATE
        hydro-for-remix.cpp
        shave-peaks-by-remix-storage-gen.cpp)

target_include_directories(${local_target}
        PUBLIC
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/solver/simulation/include>)