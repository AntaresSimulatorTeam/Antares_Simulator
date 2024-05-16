
find_package(antlr4-runtime CONFIG REQUIRED)
Set(SRCS test_antlr_interface.cpp
)

set(execname "antlr-interface-test")
add_executable(${execname}  ${SRCS})
add_compile_definitions(${execname} BOOST_TEST_DYN_LINK)
target_link_libraries(${execname}
                       PRIVATE
                       antlr-interface
		               Boost::unit_test_framework
                     )  


target_include_directories(${execname} 
        PRIVATE
         ${ANTLR4_INCLUDE_DIR})
add_test(NAME antlr-interface COMMAND ${execname})

set_tests_properties(antlr-interface PROPERTIES LABELS unit)
