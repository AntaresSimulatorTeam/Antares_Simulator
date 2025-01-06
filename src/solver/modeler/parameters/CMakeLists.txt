add_library(modelerParameters
        include/antares/solver/modeler/parameters/modelerParameters.h
        include/antares/solver/modeler/parameters/parseModelerParameters.h
        parseModelerParameters.cpp
        encoder.hxx)

add_library(Antares::modelerParameters ALIAS modelerParameters)

target_link_libraries(modelerParameters
        PRIVATE
        yaml-cpp
        Antares::io)

target_include_directories(modelerParameters
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
