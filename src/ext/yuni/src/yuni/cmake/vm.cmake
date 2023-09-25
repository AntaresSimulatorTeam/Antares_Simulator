

YMESSAGE_MODULE("Virtual Machine  (DEPRECATED)")

# Paths for yuni-config, embedded version.
LIBYUNI_CONFIG_INCLUDE_PATH("intree" "vm" "${CMAKE_CURRENT_SOURCE_DIR}/..")
LIBYUNI_CONFIG_LIB_PATH("intree" "vm" "${LIBRARY_OUTPUT_PATH}")

# Paths for yuni-config, installed version.
LIBYUNI_CONFIG_INCLUDE_PATH("target" "vm" "${CMAKE_INSTALL_PREFIX}/include/${YUNI_VERSIONED_INST_PATH}/")
LIBYUNI_CONFIG_LIB_PATH("target" "vm" "${CMAKE_INSTALL_PREFIX}/lib/${YUNI_VERSIONED_INST_PATH}/")

LIBYUNI_CONFIG_LIB("both" "vm"       "yuni-static-vm")

LIBYUNI_CONFIG_DEPENDENCY("vm" "core") # yuni-core is required


set(SRC_VM
	vm/assembly.h
	vm/assembly.hxx
	vm/fwd.h
	vm/instructions.h
	vm/program.cpp
	vm/execute.cpp
	vm/program.h
	vm/program.hxx
	vm/std.h
	)

source_group(vm FILES ${SRC_VM})


add_library(yuni-static-vm STATIC
		${SRC_VM}
	)

# Setting output path
set_target_properties(yuni-static-vm PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-vm ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install Audio-related headers
install(
	DIRECTORY vm
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)


