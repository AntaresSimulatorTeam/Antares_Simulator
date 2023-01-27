
YMESSAGE_MODULE("Marshal (Object serialization)  (EXPERIMENTAL)")

LIBYUNI_CONFIG_LIB("both" "marshal"        "yuni-static-marshal")


# Sources for parsing JSON are imported from nany and no relative paths
# is currently provided
include_directories("..")

set (SRC_PRIVATE_JSON
	private/marshal/json/grammar.cpp
	private/marshal/json/grammar.h
	private/marshal/json/grammar.hxx
	private/marshal/json/internal-charsets.hxx
	private/marshal/json/internal-dfa.hxx
	private/marshal/json/internal-lalr.hxx
	private/marshal/json/internal-productions.hxx
	private/marshal/json/internal-symbols.hxx
	private/marshal/json/internal-table.hxx
	private/marshal/json/symbols.h
)
source_group("Marshal\\Private\\JSON" FILES ${SRC_PRIVATE_JSON})

set(SRC_MARSHAL
	marshal/fwd.h
	marshal/document.h
	marshal/document.cpp
	marshal/object.h
	marshal/object.hxx
	marshal/object.cpp
)
source_group("Marshal" FILES ${SRC_MARSHAL})



add_Library(yuni-static-marshal STATIC
	${SRC_MARSHAL}
	${SRC_PRIVATE_JSON}
)

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-marshal PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-marshal ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install net-related headers
install(
	DIRECTORY marshal
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)
