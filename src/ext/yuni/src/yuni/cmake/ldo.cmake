

YMESSAGE_MODULE("Live Distributed Objects  (EXPERIMENTAL, FOR ADVANCED USERS ONLY)")

LIBYUNI_CONFIG_LIB("both" "ldo" "yuni-static-ldo")

LIBYUNI_CONFIG_DEPENDENCY("ldo" "core") # core is required
LIBYUNI_CONFIG_DEPENDENCY("ldo" "messaging") # core is required
LIBYUNI_CONFIG_DEPENDENCY("ldo" "net") # core is required


# Devices
set(SRC_LDOS
		do/do.h
		do/do.cpp
		)
source_group(ldo FILES ${SRC_LDOS})

add_library(yuni-static-ldo STATIC
			${SRC_LDOS}
)

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-ldo PROPERTIES 
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
INSTALL(TARGETS yuni-static-ldo ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

INSTALL(
	DIRECTORY do
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)

# Linking deps
target_link_libraries(yuni-static-ldo yuni-static-core)


