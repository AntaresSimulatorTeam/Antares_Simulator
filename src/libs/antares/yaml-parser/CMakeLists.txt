
find_package(yaml-cpp CONFIG REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)
Set(SRCS main.cpp
)

set(execname "yaml-parser-test")
add_executable(${execname}  ${SRCS})
# INSTALL(EXPORT ${execname}
#         FILE yaml-parser-testConfig.cmake
#         DESTINATION cmake
# )
# add antlr4cpp artifacts to project environment
target_link_libraries(${execname} PUBLIC yaml-cpp antlr4_shared) # The library or executable that require yaml-cpp 


#ugly see https://github.com/antlr/antlr4/pull/1612
get_target_property(antlr4_lib antlr4_shared IMPORTED_IMPLIB_DEBUG)
set(antlr4_include "${antlr4_lib}/../../../include/antlr4-runtime")
message ("************* ${antlr4_include} ***************** ")

target_include_directories(${execname} PUBLIC ${antlr4_include})