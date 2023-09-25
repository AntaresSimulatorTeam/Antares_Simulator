
YMESSAGE_MODULE("Media  (EXPERIMENTAL)")


LIBYUNI_CONFIG_LIB("both" "media"      "yuni-static-media-core")
LIBYUNI_CONFIG_DEPENDENCY("media" "core") # yuni-core is required


#
# Windows-specific
#
if (WIN32 OR WIN64)
	LIBYUNI_CONFIG_LIB("both" "media" ws2_32)
endif (WIN32 OR WIN64)


set(SRC_MEDIA
		media/queueservice.h
		media/queueservice.hxx
		media/queueservice.cpp
		media/loop.h
		media/loop.cpp
		media/emitter.h
		media/emitter.hxx
		media/emitter.cpp
		media/source.h
		media/source.hxx
		media/source.cpp
		private/media/file.cpp
		private/media/file.h
		private/media/file.hxx
		private/media/frame.cpp
		private/media/frame.h
		private/media/stream.h
		private/media/stream.hxx
		private/media/streamtype.h
		private/media/openal.h
		private/media/openal.cpp
		private/media/av.h
		private/media/av.cpp
	)

include(CheckIncludeFile)


#
# OpenAL
#
# Select default OpenAL mode
if(NOT "${YUNI_DvP_OPENAL_MODE}" OR YUNI_DvP_OPENAL_MODE STREQUAL "auto")
	if(WIN32 OR WIN64)
		set(OPENAL_MODE devpack)
	else()
		set(OPENAL_MODE system)
	endif()
else()
	set(OPENAL_MODE "${YUNI_DvP_OPENAL_MODE}")
endif()


YMESSAGE("      -> OpenAL Libraries: ${OPENAL_MODE}")

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/openal-${OPENAL_MODE}.cmake)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/openal-${OPENAL_MODE}.cmake)
else()
	YFATAL("[!!] Invalid OPENAL_MODE: ${OPENAL_MODE}")
endif()

YMESSAGE("      -> OpenAL Libraries: ${OPENAL_INCLUDE_DIR}")



#
# FFmpeg
#
YMESSAGE("Added Support for FFMpeg")
DEVPACK_IMPORT_FFMPEG()
set(SRC_MEDIA_FFMPEG ${YUNI_EXT_FFMPEG_HEADERS})

if (NOT WIN32 AND NOT WIN64)
	# ZLIB
	find_package(ZLIB)
	if (ZLIB_FOUND)
		list(APPEND YUNI_EXT_FFMPEG_LIB ${ZLIB_LIBRARIES})
	else (ZLIB_FOUND)
		YERROR(    "Impossible to find ZLib (Media will not work properly !)")
		YMESSAGE(    " * Packages needed on debian: libz-dev")
		YMESSAGE(    " * Packages needed on redhat: zlib-devel")
	endif (ZLIB_FOUND)

	# BZIP2
	FIND_PACKAGE(BZip2)
	if(BZIP2_FOUND)
		list(APPEND YUNI_EXT_FFMPEG_LIB ${BZIP2_LIBRARIES})
	else(BZIP2_FOUND)
		YERROR(    "Impossible to find BZip2 (Media will not work properly !)")
		YMESSAGE(    " * Packages needed on debian: libbz2-dev")
		YMESSAGE(    " * Packages needed on redhat: bzip2-devel")
	endif(BZIP2_FOUND)
endif (NOT WIN32 AND NOT WIN64)

LIBYUNI_CONFIG_LIB_RAW_COMMAND("both" "media" "${YUNI_EXT_FFMPEG_LIB}")
LIBYUNI_CONFIG_INCLUDE_PATH("both" "media" "${YUNI_EXT_FFMPEG_INCLUDE}")

### WARNING: FFmpeg 0.6 (and other versions) fail to compile with:
### error: 'UINT64_C' was not declared in this scope
### This define is required to solve this.
add_definitions(-D__STDC_CONSTANT_MACROS)
LIBYUNI_CONFIG_CFLAG("both" "media" "-D__STDC_CONSTANT_MACROS")

source_group(Media FILES ${SRC_MEDIA})
source_group(Media\\Ffmpeg FILES ${SRC_MEDIA_FFMPEG})

add_library(yuni-static-media-core STATIC ${SRC_MEDIA_FFMPEG} ${SRC_MEDIA})

# Setting output path
set_target_properties(yuni-static-media-core PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-media-core
	COMPONENT ${YUNICOMPONENT_MEDIA_CORE}
	ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install Media-related headers
install(
	DIRECTORY media
	COMPONENT ${YUNICOMPONENT_MEDIA_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)

target_link_libraries(yuni-static-media-core ${OPENAL_LIBRARY})

