
##############
##  wxWidgets  ##
##############
if(BUILD_wxWidgets)
  build_git_dependency(
    NAME
      wxWidgets
    REPOSITORY
      "https://github.com/wxWidgets/wxWidgets.git"
    TAG
      "v3.1.3"
    CMAKE_ARGS
      -DwxBUILD_SHARED=OFF
  )
endif()