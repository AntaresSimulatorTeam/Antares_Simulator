

YMESSAGE("      Using custom OpenAL package path")

if (NOT YUNI_DvP_OPENAL_PREFIX)
	YFATAL(    "[!!] Error: custom mode requires a prefix specification.")
endif()

find_path(OPENAL_INCLUDE_DIR al.h
	PATH_SUFFIXES include/AL include/OpenAL include
	PATHS ${YUNI_DvP_OPENAL_PREFIX})

find_library(OPENAL_LIBRARY 
	NAMES OpenAL al openal OpenAL32
	PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
	PATHS ${YUNI_DvP_OPENAL_PREFIX})

LIBYUNI_CONFIG_INCLUDE_PATH("both" "media" "${OPENAL_INCLUDE_DIR}")
LIBYUNI_CONFIG_LIB("both" "media" "${OPENAL_LIBRARY}")
include_directories(${OPENAL_INCLUDE_DIR})

