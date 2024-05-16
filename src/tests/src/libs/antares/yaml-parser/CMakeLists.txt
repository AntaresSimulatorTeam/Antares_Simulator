
find_package(yaml-cpp CONFIG REQUIRED)
Set(SRCS test_yaml_parser.cpp
)

set(execname "yaml-parser-test")
add_executable(${execname}  ${SRCS})
add_compile_definitions(${execname} BOOST_TEST_DYN_LINK)
target_link_libraries(${execname}
                       PRIVATE
                       yaml-cpp
		               Boost::unit_test_framework
                     )


add_test(NAME yaml-parser COMMAND ${execname})

set_tests_properties(yaml-parser PROPERTIES LABELS unit)
