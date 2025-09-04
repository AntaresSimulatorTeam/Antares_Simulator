
set(local_target "shave-peaks-by-remix-storage-gen")
set(include_dir ${CMAKE_SOURCE_DIR}/solver/simulation/include)

add_library(${local_target})

target_sources(${local_target}
        PRIVATE
		${include_dir}/antares/solver/simulation/remix-storage/remix-utils.h
		${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix-interface.h
		${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix-no-levels.h
        storage-for-remix-no-levels.cpp
		${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix-with-levels.h
        storage-for-remix-with-levels.cpp
		${include_dir}/antares/solver/simulation/remix-storage/create-storage-for-remix.h
        create-storage-for-remix.cpp
		${include_dir}/antares/solver/simulation/remix-storage/shave-peaks-by-remix-help.h
		shave-peaks-by-remix-help.cpp
		${include_dir}/antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h
        shave-peaks-by-remix-storage-gen.cpp)

target_include_directories(${local_target}
        PUBLIC
		$<BUILD_INTERFACE:${include_dir}>)

set_target_properties(${local_target} PROPERTIES FOLDER storage-remix)

target_link_libraries(${local_target} PRIVATE storage-for-remix-list-sort)

# Lib : sorting list of storage for remix
add_library(storage-for-remix-list-sort)

target_sources(storage-for-remix-list-sort
			   PRIVATE
			   ${include_dir}/antares/solver/simulation/remix-storage/storage-for-remix-interface.h
			   storage-for-remix-list-sort.cpp)

target_include_directories(storage-for-remix-list-sort
        PUBLIC
		$<BUILD_INTERFACE:${include_dir}>)

set_target_properties(storage-for-remix-list-sort PROPERTIES FOLDER storage-remix)