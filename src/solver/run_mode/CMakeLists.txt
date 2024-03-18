add_library(run-mode)
add_library(Antares::run-mode ALIAS run-mode)
target_sources(run-mode PRIVATE
        economy.cpp
        adequacy.cpp
        include/antares/solver/run_mode/economy.h
        include/antares/solver/run_mode/adequacy.h
)

target_link_libraries(run-mode
        PRIVATE
        Antares::infoCollection
        Antares::application
        antares-solver-simulation
)

target_include_directories(model_antares
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)