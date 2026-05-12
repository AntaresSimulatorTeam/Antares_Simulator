set(SRC
        generationAndResolutionConfig.cpp
        include/antares/solver/workflow/generationAndResolutionConfig.h
)

add_library(workflow ${SRC})
add_library(Antares::workflow ALIAS workflow)

target_link_libraries(workflow
        PRIVATE
        Antares::study
        modeler-lib
        model_antares
)

target_include_directories(workflow
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
