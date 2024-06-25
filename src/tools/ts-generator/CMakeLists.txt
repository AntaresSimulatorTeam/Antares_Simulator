set(SRCS
        main.cpp
		include/antares/tools/ts-generator/linksTSgenerator.h
		include/antares/tools/ts-generator/tsGenerationOptions.h
		tsGenerationOptions.cpp
		linksTSgenerator.cpp
)

set(execname "antares-ts-generator")
add_executable(${execname} ${SRCS})
install(TARGETS ${execname} EXPORT antares-ts-generator DESTINATION bin)

INSTALL(EXPORT ${execname}
        FILE antares-ts-generatorConfig.cmake
        DESTINATION cmake
)

target_link_libraries(${execname}
                      PRIVATE
						Antares::utils
						antares-solver-ts-generator
						Antares::study
						Antares::checks
)

target_include_directories(${execname}
        PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

import_std_libs(${execname})
executable_strip(${execname})
