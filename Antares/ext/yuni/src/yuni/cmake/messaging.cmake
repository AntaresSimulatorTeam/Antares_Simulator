
YMESSAGE_MODULE("Messaging  (EXPERIMENTAL)")

LIBYUNI_CONFIG_LIB("both" "messaging"        "yuni-static-messaging")




set(SRC_NET_MESSAGE
	messaging/service.h
	messaging/service.cpp
	messaging/service.hxx
	messaging/fwd.h

	messaging/worker.inc.hpp
	messaging/protocol.h
	messaging/protocol.hxx
	messaging/protocol.cpp
	messaging/schema.h
	messaging/schema.cpp
	messaging/schema.hxx
	messaging/message.h
	messaging/threadcontext.h
	messaging/threadcontext.cpp
	messaging/threadcontext.hxx
)
source_group("Messaging" FILES ${SRC_NET_MESSAGE})

set(SRC_NET_MESSAGE_TRANSPORT
	messaging/transport.h
	messaging/transport/transport.h
	messaging/transport/transport.hxx
)
source_group("Messaging\\Transport" FILES ${SRC_NET_MESSAGE_TRANSPORT})

set(SRC_NET_MESSAGE_TRANSPORT_REST
	messaging/transport/rest/server.h
	messaging/transport/rest/server.cpp
	messaging/transport/rest/request.inc.hpp
)
source_group("Messaging\\Transport\\REST" FILES ${SRC_NET_MESSAGE_TRANSPORT_REST})

set(SRC_NET_MESSAGE_API
	messaging/api/methods.cpp
	messaging/api/methods.h
	messaging/api/methods.hxx
	messaging/api/fwd.h
	messaging/api/method.h
	messaging/api/method.cpp
	messaging/api/method.hxx
)
source_group("Messaging\\API" FILES ${SRC_NET_MESSAGE_API})


add_Library(yuni-static-messaging STATIC
	${SRC_NET_MESSAGE}
	${SRC_NET_MESSAGE_API}
	${SRC_NET_MESSAGE_TRANSPORT}
	${SRC_NET_MESSAGE_TRANSPORT_REST}
)

# Setting output path
SET_TARGET_PROPERTIES(yuni-static-messaging PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-messaging ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install net-related headers
install(
	DIRECTORY messaging
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)

