set(PROJ stdcxx)
set(HEADERS
        include/antares/${PROJ}/demangle.hpp
)
set(SRC_PROJ
        ${HEADERS}
        demangle.cpp
)
source_group("stdcxx" FILES ${SRC_STDCXX})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

#target_link_libraries(${PROJ}
#        PRIVATE
#)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
