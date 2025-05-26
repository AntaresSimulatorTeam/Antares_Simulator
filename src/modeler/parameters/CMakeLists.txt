add_library(modeler-parameters
        include/antares/solver/modeler/parameters/modelerParameters.h
        include/antares/solver/modeler/parameters/parseModelerParameters.h
        parseModelerParameters.cpp
        encoder.hxx)

add_library(Antares::modeler-parameters ALIAS modeler-parameters)

target_link_libraries(modeler-parameters
        PRIVATE
        yaml-cpp::yaml-cpp
        Antares::io)

target_include_directories(modeler-parameters
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
