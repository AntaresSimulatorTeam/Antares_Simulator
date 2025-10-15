set(SOURCES
        include/antares/io/inputs/data-series-csv-importer/DataSeriesRepoImporter.h
        DataSeriesRepoImporter.cpp
)

# Create the library
add_library(data-series-csv-importer STATIC ${SOURCES})
add_library(Antares::data-series-csv-importer ALIAS data-series-csv-importer)

# Specify include directories
target_include_directories(data-series-csv-importer
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
find_package(Boost REQUIRED COMPONENTS iostreams)
# Link dependencies
target_link_libraries(data-series-csv-importer
        PUBLIC
        Antares::linear-problem-data-impl
        PRIVATE
        Boost::headers
        Boost::iostreams
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
