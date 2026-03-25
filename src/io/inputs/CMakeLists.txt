OMESSAGE("Antares IO-Inputs")

# Header-only INTERFACE target exposing InputError
add_library(input-error INTERFACE)
add_library(Antares::input-error ALIAS input-error)
target_include_directories(input-error
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(input-error
        INTERFACE
        Antares::exception
)
install(DIRECTORY include/antares
        DESTINATION "include"
)

add_subdirectory(forbidden-nodes)
add_subdirectory(yml-utils)
add_subdirectory(model-converter)
add_subdirectory(yml-model)
add_subdirectory(yml-optim-config)
add_subdirectory(yml-system)
add_subdirectory(data-series-csv-importer)
