# build_git_dependency()
#
# CMake function to download, build and install (in staging area) a dependency at configure
# time.
#
# Parameters:
# NAME: name of the dependency
# REPOSITORY: git url of the dependency
# TAG: tag of the dependency
# APPLY_PATCH: apply patch
# CMAKE_ARGS: List of specific CMake args to add
#
# build_dependency(
#   NAME
#     abseil-cpp
#   URL
#     https://github.com/abseil/abseil-cpp.git
#   TAG
#     master
#   APPLY_PATCH
#     ${CMAKE_SOURCE_DIR}/patches/abseil-cpp.patch
# )
function(build_git_dependency)
  set(options "")
  set(oneValueArgs NAME REPOSITORY TAG APPLY_PATCH)
  set(multiValueArgs CMAKE_ARGS)
  cmake_parse_arguments(GIT_DEP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )
  message(STATUS "Building ${GIT_DEP_NAME}: ...")

  if(GIT_DEP_APPLY_PATCH)
    set(PATCH_CMD "git apply \"${GIT_DEP_APPLY_PATCH}\"")
  else()
    set(PATCH_CMD "\"\"")
  endif()
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in
    ${CMAKE_CURRENT_BINARY_DIR}/${GIT_DEP_NAME}/CMakeLists.txt @ONLY)

  execute_process(
    COMMAND ${CMAKE_COMMAND} -H. -Bproject_build -G "${CMAKE_GENERATOR}"
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${GIT_DEP_NAME})
  if(result)
    message(FATAL_ERROR "CMake step for ${GIT_DEP_NAME} failed: ${result}")
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} --build project_build --config ${CMAKE_BUILD_TYPE}
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${GIT_DEP_NAME})
  if(result)
    message(FATAL_ERROR "Build step for ${GIT_DEP_NAME} failed: ${result}")
  endif()

  message(STATUS "Building ${GIT_DEP_NAME}: ...DONE")
endfunction()

