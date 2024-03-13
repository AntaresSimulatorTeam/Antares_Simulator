
find_package(yaml-cpp CONFIG REQUIRED)
Set(SRCS main.cpp
)

set(execname "yaml-parser-test")
add_executable(${execname}  ${SRCS})
# INSTALL(EXPORT ${execname}
#         FILE yaml-parser-testConfig.cmake
#         DESTINATION cmake
# )
# add antlr4cpp artifacts to project environment
target_link_libraries(${execname} PUBLIC yaml-cpp ) # The library or executable that require yaml-cpp 


