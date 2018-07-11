
YMESSAGE_MODULE("Tests")

LIBYUNI_CONFIG_LIB("both" "tests"      "yuni-static-tests")


set(SRC_TESTS
		test/test.h  test/checkpoint.h test/checkpoint.cpp)
source_group(Tests FILES ${SRC_TESTS})



add_library(yuni-static-tests STATIC  yuni.h ${SRC_TESTS})

# Setting output path
set_target_properties(yuni-static-tests PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

