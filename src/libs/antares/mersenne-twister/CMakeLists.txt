set(PROJ mersenne)
set(SRC_PROJ
        mersenne-twister.cpp
        include/antares/mersenne-twister/mersenne-twister.h
        include/antares/mersenne-twister/mersenne-twister.hxx
)
source_group("mersenne-twister" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
