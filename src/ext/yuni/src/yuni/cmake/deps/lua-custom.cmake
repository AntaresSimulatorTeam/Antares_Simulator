
YMESSAGE("      Using custom Lua package path")

if (NOT YUNI_DvP_LUA_PREFIX)
		YFATAL(    "[!!] Error: custom mode requires a prefix specification.")
endif()

YMESSAGE("      Using Lua from custom prefix: ${YUNI_DvP_LUA_PREFIX}")

# Set internal include path
Set(YUNI_EXT_LUA_INCLUDE "${YUNI_DvP_LUA_PREFIX}/include")

# Set Yuni-Config stuff
LIBYUNI_CONFIG_INCLUDE_PATH("lua" "${YUNI_DvP_LUA_PREFIX}/include")
LIBYUNI_CONFIG_LIB_PATH("lua" "${YUNI_DvP_LUA_PREFIX}/lib")
LIBYUNI_CONFIG_LIB("lua" "lua")

