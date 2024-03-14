
find_package(yaml-cpp CONFIG REQUIRED)
Set(SRCS test_yaml_parser.cpp
)

set(execname "yaml-parser-test")
add_executable(${execname}  ${SRCS})
# INSTALL(EXPORT ${execname}
#         FILE yaml-parser-testConfig.cmake
#         DESTINATION cmake
# )
# add antlr4cpp artifacts to project environment
target_link_libraries(${execname}
                       PRIVATE
                       yaml-cpp
		               Boost::unit_test_framework
                     ) # The library or executable that require yaml-cpp 


add_test(NAME yaml-parser COMMAND ${execname})

set_tests_properties(yaml-parser PROPERTIES LABELS unit)
