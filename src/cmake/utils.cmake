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
# CONFIGURE_COMMAND : Command used for configure (default empty and default CMake configure use)
# BUILD_COMMAND : Command used for build (default empty and default CMake build use)
# INSTALL_COMMAND) : Command used for install (default empty and default CMake instal use)
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
  set(oneValueArgs NAME REPOSITORY TAG APPLY_PATCH CONFIGURE_COMMAND BUILD_COMMAND INSTALL_COMMAND)
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

  if(GIT_DEP_CONFIGURE_COMMAND)
    set(CONFIGURE_COMMAND "CONFIGURE_COMMAND ${GIT_DEP_CONFIGURE_COMMAND}")
  else()
    set(CONFIGURE_COMMAND "#CONFIGURE_COMMAND")
  endif()
  
  if(GIT_DEP_BUILD_COMMAND)
    set(BUILD_COMMAND "BUILD_COMMAND ${GIT_DEP_BUILD_COMMAND}")
  else()
    set(BUILD_COMMAND "#BUILD_COMMAND")
  endif()
  
  if(GIT_DEP_INSTALL_COMMAND)
    set(INSTALL_COMMAND "INSTALL_COMMAND ${GIT_DEP_INSTALL_COMMAND}")
  else()
    set(INSTALL_COMMAND "#INSTALL_COMMAND")
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

