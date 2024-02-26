# set(SRCS main.cpp)

set(execname "antares-timeseries-generator")
add_executable(${execname}  main.cpp)
install(TARGETS ${execname} EXPORT antares-timeseries-generator DESTINATION bin)

INSTALL(EXPORT antares-timeseries-generator
        FILE antares-timeseries-generatorConfig.cmake
        DESTINATION cmake
)


target_link_libraries(${execname}
                      PRIVATE
                      antares-solver-ts-generator
                      Antares::study)

import_std_libs(${execname})
executable_strip(${execname})
