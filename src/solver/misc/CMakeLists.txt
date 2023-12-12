set(PROJ misc)
set(HEADERS
        include/antares/solver/misc/cholesky.h
        include/antares/solver/misc/cholesky.hxx
        include/antares/solver/misc/matrix-dp-make.h
        include/antares/solver/misc/matrix-dp-make.hxx
        include/antares/solver/misc/options.h
        include/antares/solver/misc/system-memory.h
        include/antares/solver/misc/write-command-line.h
)
set(SRC_MISC
        ${HEADERS}
        options.cpp
        system-memory.cpp
        write-command-line.cpp
)
source_group("solver\\misc" FILES ${SRC_MISC})

add_library(${PROJ} ${SRC_MISC})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        Antares::logs
        Antares::exception
        Antares::utils
        PUBLIC
        yuni-static-core
        Antares::study
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
