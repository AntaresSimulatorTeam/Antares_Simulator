set(SRCS
        main.cpp
)

set(execname "antares-timeseries-generator")
add_executable(${execname} ${SRCS})
install(TARGETS ${execname} EXPORT antares-timeseries-generator DESTINATION bin)

INSTALL(EXPORT ${execname}
        FILE antares-timeseries-generatorConfig.cmake
        DESTINATION cmake
)

target_link_libraries(${execname}
                      PRIVATE
                      Antares::utils
                      antares-solver-ts-generator
                      Antares::study
)

target_include_directories(${execname}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

import_std_libs(${execname})
executable_strip(${execname})
