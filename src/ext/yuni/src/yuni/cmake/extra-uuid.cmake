
YMESSAGE_MODULE("UUID")

LIBYUNI_CONFIG_LIB("both" "uuid" "yuni-static-uuid")
LIBYUNI_CONFIG_DEPENDENCY("uuid" "core") # core is required

if(NOT APPLE AND UNIX)
	# -luuid
	LIBYUNI_CONFIG_LIB("both" "uuid" "uuid")
endif()

if (WIN32 OR WIN64)
	# -lole32
	LIBYUNI_CONFIG_LIB("both" "uuid" "ole32")
	# -lrpcrt4
	LIBYUNI_CONFIG_LIB("both" "uuid" "rpcrt4")
endif()

add_library(yuni-static-uuid STATIC
	uuid/uuid.h
	uuid/uuid.hxx
	uuid/uuid.cpp
	uuid/fwd.h
	yuni.h
)

if (WIN32 OR WIN64)
	# -lrpcrt4
	target_link_libraries(yuni-static-uuid PUBLIC rpcrt4)
	# -lole32
	target_link_libraries(yuni-static-uuid PUBLIC ole32)
endif()

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-uuid PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-uuid
	COMPONENT ${YUNICOMPONENT_UUID}
	ARCHIVE DESTINATION lib)

# Install UUID-related headers
install(
	DIRECTORY uuid
	COMPONENT ${YUNICOMPONENT_UUID}
	DESTINATION include/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)


