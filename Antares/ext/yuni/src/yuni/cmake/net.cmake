
YMESSAGE_MODULE("Network  (EXPERIMENTAL)")

LIBYUNI_CONFIG_LIB("both" "net"        "yuni-static-net")


set(SRC_NET_COMMON
	net/net.h
	net/errors.h
	net/errors.cpp
	net/port.h
	net/port.hxx
	net/hostaddressport.h
	net/hostaddressport.hxx
	net/protocol.h
	net/protocol.cpp
	net/http-status-code.cpp
	net/http-status-code.h
)
source_group("Net" FILES ${SRC_NET_COMMON})



set(SRC_PRIVATE_NET_MESSAGE_TRANSPORT_REST
	private/net/messaging/transport/rest/mongoose.c
	private/net/messaging/transport/rest/mongoose.h
)
source_group("Private\\Net\\Messaging\\Transport\\REST" FILES ${SRC_PRIVATE_NET_MESSAGE_TRANSPORT_REST})

if (YUNI_HAS_STDINT_H)
	set_source_files_properties(private/net/messaging/transport/rest/mongoose.c PROPERTIES COMPILE_FLAGS -DHAVE_STDINT)
endif()
if (YUNI_HAS_GCC_NOWARNING)
	set_source_files_properties(private/net/messaging/transport/rest/mongoose.c PROPERTIES COMPILE_FLAGS -w)
endif()
if (YUNI_HAS_VS_NOWARNING)
	set_source_files_properties(private/net/messaging/transport/rest/mongoose.c PROPERTIES COMPILE_FLAGS /nowarn)
endif()




add_Library(yuni-static-net STATIC
	${SRC_NET_COMMON}
	${SRC_PRIVATE_NET_MESSAGE_TRANSPORT_REST}
)

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-net PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-net ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install net-related headers
install(
	DIRECTORY net
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)
