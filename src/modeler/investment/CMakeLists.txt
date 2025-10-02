add_library(modeler-optim-config
            include/antares/modeler/investment/optimConfig/optimConfig.h
            optimConfig/optimConfig.cpp)

add_library(Antares::modeler-optim-config ALIAS modeler-optim-config)

target_link_libraries(modeler-optim-config
        PRIVATE
        Antares::exception
)

target_include_directories(modeler-optim-config
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


add_library(investment
            include/antares/modeler/investment/investment.h
            investment.cpp)

add_library(Antares::investment ALIAS investment)

target_link_libraries(investment
        PRIVATE
        Antares::modeler-optim-config
        Antares::modeler-lib
        Antares::exception
        Antares::logs
)

target_include_directories(investment
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

