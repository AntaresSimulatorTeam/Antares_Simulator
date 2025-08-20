
set(local_target "shave-peaks-by-remix-storage-gen")
set(include_dir ${CMAKE_SOURCE_DIR}/solver/simulation/include)

add_library(${local_target})

target_sources(${local_target}
        PRIVATE
		${include_dir}/antares/solver/simulation/remix-storage/remix-utils.h
		${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix-interface.h
		${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix.h
        storage-for-remix.cpp
		${include_dir}/antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h
        shave-peaks-by-remix-storage-gen.cpp)

target_include_directories(${local_target}
        PUBLIC
		$<BUILD_INTERFACE:${include_dir}>)