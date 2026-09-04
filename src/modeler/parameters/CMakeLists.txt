add_library(modeler-parameters
        include/antares/solver/modeler/parameters/modelerParameters.h
        include/antares/solver/modeler/parameters/parseModelerParameters.h
        include/antares/solver/modeler/parameters/scenarioScope.h
        parseModelerParameters.cpp
        scenarioScope.cpp
        decoder.hxx)

add_library(Antares::modeler-parameters ALIAS modeler-parameters)

target_link_libraries(modeler-parameters
        PRIVATE
        yaml-cpp::yaml-cpp
        Antares::io
        Antares::logs
        nlohmann_json::nlohmann_json)

target_include_directories(modeler-parameters
        PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
