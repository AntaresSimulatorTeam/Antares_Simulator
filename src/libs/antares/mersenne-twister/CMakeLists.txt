set(PROJ mersenne)
set(SRC_PROJ
        mersenne-twister.cpp
        include/antares/mersenne-twister/mersenne-twister.h
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

if (${BUILD_MERSENNE_TWISTER_PYBIND11})
    pybind11_add_module(mersenne_twister_pybind11 mersenne_twister_pybind11.cpp)
    target_link_libraries(mersenne_twister_pybind11
        PRIVATE
        ${PROJ}
        yuni-static-core)

    # Fix for clang+pybind11
    # See https://github.com/pybind/pybind11/issues/1604
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(mersenne_twister_pybind11 PRIVATE -fsized-deallocation)
    endif()
endif()
