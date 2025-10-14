add_library(modeler-optim-config
            include/antares/modeler/optimConfig/optimConfig.h
            optimConfig.cpp)

add_library(Antares::modeler-optim-config ALIAS modeler-optim-config)

target_link_libraries(modeler-optim-config
        PRIVATE
        Antares::exception
)

target_include_directories(modeler-optim-config
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
