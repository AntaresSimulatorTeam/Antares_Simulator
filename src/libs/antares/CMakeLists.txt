
OMESSAGE("Antares Core library")

add_subdirectory(InfoCollection)
add_subdirectory(args)
add_subdirectory(array)
add_subdirectory(benchmarking)
add_subdirectory(checks)
add_subdirectory(concurrency)
add_subdirectory(correlation)
add_subdirectory(date)
add_subdirectory(exception)
add_subdirectory(file-tree-study-loader)
add_subdirectory(inifile)
add_subdirectory(io)
add_subdirectory(jit)
add_subdirectory(locale)
add_subdirectory(locator)
add_subdirectory(logs)
add_subdirectory(memory)
add_subdirectory(mersenne-twister)
add_subdirectory(paths)
add_subdirectory(resources)
add_subdirectory(series)
add_subdirectory(stdcxx)
add_subdirectory(study)
add_subdirectory(study-loader)
add_subdirectory(sys)
add_subdirectory(utils)
add_subdirectory(writer)
add_subdirectory(antlr-interface)

add_subdirectory(optimization-options)

set(HEADERS
        include/antares/antares/antares.h
        include/antares/antares/constants.h
        include/antares/antares/fatal-error.h
        include/antares/antares/version.h
        include/antares/antares/Enum.hpp
        include/antares/antares/Enum.hxx
)
set(SRC
        ${HEADERS}
        constants.cpp
        version.cpp
)


add_library(antares-core
        ${SRC}
)

target_include_directories(antares-core
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libs>
)

target_link_libraries(antares-core
        PUBLIC
            yuni-static-core
            Antares::logs
            Antares::exception
            Antares::study
            Antares::config
)

import_std_libs(antares-core)

install(DIRECTORY include/antares
        DESTINATION "include"
)
