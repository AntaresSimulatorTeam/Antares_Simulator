set(PROJ gems-csr-adapter)

set(SRC_gems_csr_adapter
        include/antares/solver/adequacy-patch/gems-csr-adapter.h
        gems-csr-adapter.cpp
)

add_library(${PROJ} ${SRC_gems_csr_adapter})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        Antares::antares-study-system-model
        Antares::linear-problem-api
        PRIVATE
        Antares::expressions
        Antares::logs
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)