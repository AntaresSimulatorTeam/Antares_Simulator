
YMESSAGE_MODULE("Parser Generator")

LIBYUNI_CONFIG_LIB("both" "parser"        "yuni-static-parser")

include_directories("..")


add_executable(yuni-tool-parser-template EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/cmake/parser/importer.cpp")

target_link_libraries(yuni-tool-parser-template yuni-static-core)
set_target_properties(yuni-tool-parser-template PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/bin")


add_custom_command(
	OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/private/parser/peg/__parser.include.cpp.hxx"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/private/parser/peg/__parser.include.cpp.template"
	        yuni-tool-parser-template
	COMMAND "$<TARGET_FILE:yuni-tool-parser-template>"
		"${CMAKE_CURRENT_SOURCE_DIR}/private/parser/peg/__parser.include.cpp.template"
		"${CMAKE_CURRENT_BINARY_DIR}/private/parser/peg/__parser.include.cpp.hxx")




set(SRC_PARSER
	parser/peg/grammar.h
	parser/peg/grammar.hxx
	parser/peg/grammar.cpp
	parser/peg/node.h
	parser/peg/node.hxx
	parser/peg/node.cpp
	parser/peg/export-cpp.cpp
	parser/peg/export-dot.cpp
	"${CMAKE_CURRENT_BINARY_DIR}/private/parser/peg/__parser.include.cpp.hxx"
)
source_group("Parser\\Generator" FILES ${SRC_PARSER})



add_Library(yuni-static-parser STATIC ${SRC_PARSER})

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-parser PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-parser ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install net-related headers
install(
	DIRECTORY parser
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE)
