set(SRCS
        main.cpp
        include/antares/timeseries-generation/timeseries-generation.h
)

set(execname "antares-timeseries-generator")
add_executable(${execname} ${SRCS})
install(TARGETS ${execname} EXPORT antares-timeseries-generator DESTINATION bin)

INSTALL(EXPORT antares-timeseries-generator
        FILE antares-timeseries-generatorConfig.cmake
        DESTINATION cmake
)

target_link_libraries(${execname}
                      PRIVATE
                      antares-solver-ts-generator
                      Antares::study
)

target_include_directories(${execname}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
import_std_libs(${execname})
executable_strip(${execname})
