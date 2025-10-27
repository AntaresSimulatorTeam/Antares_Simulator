set(CMAKE_UNITY_BUILD ON)
add_subdirectory(api_internal)
add_subdirectory(api_lib)
add_subdirectory(utils)

add_subdirectory(libs)

add_subdirectory(solver)

add_subdirectory(expressions)
add_subdirectory(optimisation)
add_subdirectory(study)
add_subdirectory(io)
add_subdirectory(modeler)