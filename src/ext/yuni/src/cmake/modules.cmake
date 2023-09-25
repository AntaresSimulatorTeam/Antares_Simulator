
#
# --- Modules - Default settings ---
#
if (WIN32 OR WIN64)
	# NSIS does not support semicolons.....
	set(YUNICOMPONENT_CORE              "yuni_core")
	set(YUNICOMPONENT_ALGORITHMS        "yuni_algorithms")
	set(YUNICOMPONENT_MEDIA_CORE        "yuni_media_core")
	set(YUNICOMPONENT_DEVICE_DISPLAY    "yuni_device_display")
	set(YUNICOMPONENT_UUID              "yuni_uuid")
	set(YUNICOMPONENT_MARSHAL           "yuni_marshal")
	set(YUNICOMPONENT_NET               "yuni_net")
	set(YUNICOMPONENT_MESSAGING         "yuni_messaging")
	set(YUNICOMPONENT_DBI               "yuni_dbi")
	set(YUNICOMPONENT_PARSER            "yuni_parser")
else()
	set(YUNICOMPONENT_CORE              "yuni-core")
	set(YUNICOMPONENT_ALGORITHMS        "yuni-algorithms")
	set(YUNICOMPONENT_MEDIA_CORE        "yuni-media-core")
	set(YUNICOMPONENT_DEVICE_DISPLAY    "yuni-device-display")
	set(YUNICOMPONENT_UUID              "yuni-uuid")
	set(YUNICOMPONENT_MARSHAL           "yuni-marshal")
	set(YUNICOMPONENT_NET               "yuni-net")
	set(YUNICOMPONENT_MESSAGING         "yuni-messaging")
	set(YUNICOMPONENT_DBI               "yuni-dbi")
	set(YUNICOMPONENT_PARSER            "yuni-parser")
endif()




# Core
set(YUNI_MODULE_CORE                      true) # Must be True

# benchmarks
set(YUNI_BENCHMARKS                       false)

# VFS
#set(YUNI_MODULE_VFS                       false)
#	set(YUNI_MODULE_VFS_FILE              true)

# VM
set(YUNI_MODULE_VM                        false)

# Marshal
set(YUNI_MARSHAL                          false)

# Devices
set(YUNI_MODULE_DEVICES                   false)
	set(YUNI_MODULE_DEVICE_DISPLAY        true)
	set(YUNI_MODULE_DEVICE_KEYBOARD       true)
	set(YUNI_MODULE_DEVICE_MOUSE          true)

set(YUNI_MODULE_DBI                       false)
	set(YUNI_MODULE_DBI_SQLITE            false)

# Media
set(YUNI_MODULE_MEDIA                     false)

# Network
set(YUNI_MODULE_NET                       false)

# Messaging
set(YUNI_MODULE_MESSAGING                 false)

# LDO
set(YUNI_MODULE_LDO                       false)

# Graphics
set(YUNI_MODULE_GRAPHICS                  false)
set(YUNI_MODULE_OPENGL                    false)

# UI (User Interface)
set(YUNI_MODULE_UI                        false)

# Algorithms
set(YUNI_MODULE_ALGORITHMS                false)

# UUID
set(YUNI_MODULE_EXTRA_UUID                false)

# Doc
set(YUNI_MODULE_DOCUMENTATION             false)

# Parser Generator 
set(YUNI_MODULE_PARSER                    false)

# Tests
set(YUNI_TESTS   false)






# The list of all available modules
# There is no need for `core`, which is implicit
set(YUNI_MODULE_LIST
	algorithms
	vm
	marshal
	vfs
		vfs-local
	media
	devices
		display
		keyboard
		mouse
	graphics
		opengl
	ui
	dbi
		dbi-sqlite
	net
		netserver
		netclient
	ldo
	# extra
		uuid
	docs
	parser
	benchmarks
)




#
# --- Command lines options ---
#
if (MODULES)
	set(KeywordError false)
	string(REPLACE "," ";" MODULES "${MODULES}")
	string(REPLACE " " ";" MODULES "${MODULES}")
	string(REPLACE "+" "" MODULES "${MODULES}")

	foreach(it ${MODULES})
		set(KeywordIsKnown false)

		# core
		if ("${it}" STREQUAL "core")
			set(YUNI_MODULE_CORE true)
			set(KeywordIsKnown true)
		endif()
		# -core
		if ("${it}" STREQUAL "-core")
			set(KeywordIsKnown true)
			YMESSAGE("[!!] Module: Impossible to disable the core module")
			set(KeywordError true)
		endif()

		# all
		if ("${it}" STREQUAL "all")
			set(YUNI_MODULE_CORE true)
			#set(YUNI_MODULE_VFS true)
			set(YUNI_MODULE_DEVICES true)
			set(YUNI_MODULE_VM true)
			set(YUNI_MODULE_MEDIA true)
			set(YUNI_MODULE_NET true)
			set(YUNI_MODULE_MESSAGING true)
			set(YUNI_MODULE_GRAPHICS true)
			set(YUNI_MODULE_OPENGL true)
			set(YUNI_MODULE_UI true)
			set(YUNI_MODULE_ALGORITHMS true)
			set(YUNI_MODULE_EXTRA_UUID true)
			set(YUNI_MODULE_LDO true)
			set(YUNI_MODULE_DOCUMENTATION true)
			set(YUNI_MODULE_MARSHAL true)
			set(YUNI_MODULE_DBI true)
			set(YUNI_MODULE_PARSER true)
			set(YUNI_TESTS true)
			set(YUNI_BENCHMARKS true)
			set(KeywordIsKnown true)
		endif()

		# vfs
		#if ("${it}" STREQUAL "vfs")
		#	set(YUNI_MODULE_VFS true)
		#	set(KeywordIsKnown true)
		#endif()
		# -vfs
		#if ("${it}" STREQUAL "-vfs")
		#	set(YUNI_MODULE_VFS false)
		#	set(KeywordIsKnown true)
		#endif()

		# vfs-local
		#if ("${it}" STREQUAL "vfs-local")
		#	set(YUNI_MODULE_VFS true)
		#	set(YUNI_MODULE_VFS_LOCAL true)
		#	set(KeywordIsKnown true)
		#endif()
		# -vfs
		#if ("${it}" STREQUAL "-vfs-local")
		#	set(YUNI_MODULE_VFS_LOCAL false)
		#	set(KeywordIsKnown true)
		#endif()

		# benchmarks
		if ("${it}" STREQUAL "benchmarks")
			set(YUNI_BENCHMARKS true)
			set(KeywordIsKnown true)
		endif()
		# -benchmarks
		if ("${it}" STREQUAL "-benchmarks")
			set(YUNI_BENCHMARKS false)
			set(KeywordIsKnown true)
		endif()

		# vm
		if ("${it}" STREQUAL "vm")
			set(YUNI_MODULE_VM true)
			set(KeywordIsKnown true)
		endif()
		# -vm
		if ("${it}" STREQUAL "-vm")
			set(YUNI_MODULE_VM false)
			set(KeywordIsKnown true)
		endif()

		# dbi
		if ("${it}" STREQUAL "dbi")
			set(YUNI_MODULE_DBI true)
			set(KeywordIsKnown true)
		endif()
		# -dbi
		if ("${it}" STREQUAL "-dbi")
			set(YUNI_MODULE_DBI false)
			set(KeywordIsKnown true)
		endif()

		# dbi-sqlite
		if ("${it}" STREQUAL "dbi-sqlite")
			set(YUNI_MODULE_DBI_SQLITE true)
			set(KeywordIsKnown true)
		endif()
		# -dbi-sqlite
		if ("${it}" STREQUAL "-dbi-sqlite")
			set(YUNI_MODULE_DBI_SQLITE false)
			set(KeywordIsKnown true)
		endif()

		# marshal
		if ("${it}" STREQUAL "marshal")
			set(YUNI_MODULE_MARSHAL true)
			set(KeywordIsKnown true)
		endif()
		# -marshal
		if ("${it}" STREQUAL "-marshal")
			set(YUNI_MODULE_MARSHAL false)
			set(KeywordIsKnown true)
		endif()

		# messaging
		if ("${it}" STREQUAL "messaging")
			set(YUNI_MODULE_MESSAGING true)
			set(KeywordIsKnown true)
		endif()
		# -messaging
		if ("${it}" STREQUAL "-messaging")
			set(YUNI_MODULE_MESSAGING false)
			set(KeywordIsKnown true)
		endif()

		# ldo
		if ("${it}" STREQUAL "ldo")
			set(YUNI_MODULE_LDO true)
			set(KeywordIsKnown true)
		endif()
		# -ldo
		if ("${it}" STREQUAL "-ldo")
			set(YUNI_MODULE_LDO false)
			set(KeywordIsKnown true)
		endif()

		# algorithms
		if ("${it}" STREQUAL "algorithms")
			set(YUNI_MODULE_ALGORITHMS true)
			set(KeywordIsKnown true)
		endif()
		# -algorithms
		if ("${it}" STREQUAL "-algorithms")
			set(YUNI_MODULE_ALGORITHMS false)
			set(KeywordIsKnown true)
		endif()


		# display
		if ("${it}" STREQUAL "display")
			set(YUNI_MODULE_DEVICE_DISPLAY true)
			set(KeywordIsKnown true)
		endif()
		# -display
		if ("${it}" STREQUAL "-display")
			set(YUNI_MODULE_DEVICE_DISPLAY false)
			set(KeywordIsKnown true)
		endif()

		# keyboard
		if ("${it}" STREQUAL "keyboard")
			set(YUNI_MODULE_DEVICE_KEYBOARD true)
			set(KeywordIsKnown true)
		endif()
		# -keyboard
		if ("${it}" STREQUAL "-keyboard")
			set(YUNI_MODULE_DEVICE_KEYBOARD false)
			set(KeywordIsKnown true)
		endif()

		# mouse
		if ("${it}" STREQUAL "mouse")
			set(YUNI_MODULE_DEVICE_MOUSE true)
			set(KeywordIsKnown true)
		endif()
		# -mouse
		if ("${it}" STREQUAL "-mouse")
			set(YUNI_MODULE_DEVICE_MOUSE false)
			set(KeywordIsKnown true)
		endif()

		# devices
		if ("${it}" STREQUAL "devices")
			set(YUNI_MODULE_DEVICES true)
			set(KeywordIsKnown true)
		endif()
		# -devices
		if ("${it}" STREQUAL "-devices")
			set(YUNI_MODULE_DEVICES false)
			set(KeywordIsKnown true)
		endif()


		# net
		if ("${it}" STREQUAL "net")
			set(YUNI_MODULE_NET true)
			set(KeywordIsKnown true)
		endif()
		# -net
		if ("${it}" STREQUAL "-net")
			set(YUNI_MODULE_NET false)
			set(KeywordIsKnown true)
		endif()

		# media
		if ("${it}" STREQUAL "media")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_MEDIA true)
		endif()
		# -media
		if ("${it}" STREQUAL "-media")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_MEDIA false)
		endif()


		# Tests
		if ("${it}" STREQUAL "tests")
			set(KeywordIsKnown true)
			set(YUNI_TESTS true)
		endif()
		# -tests
		if ("${it}" STREQUAL "-tests")
			set(KeywordIsKnown true)
			set(YUNI_TESTS false)
		endif()

		# graphics (Graphic renderers)
		if ("${it}" STREQUAL "graphics")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_GRAPHICS true)
		endif()
		# -graphics
		if ("${it}" STREQUAL "-graphics")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_GRAPHICS false)
		endif()

		# OpenGL renderer
		if ("${it}" STREQUAL "opengl")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_OPENGL true)
		endif()
		# -opengl
		if ("${it}" STREQUAL "-opengl")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_OPENGL false)
		endif()

		# ui (User Interface)
		if ("${it}" STREQUAL "ui")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_UI true)
		endif()
		# -ui
		if ("${it}" STREQUAL "-ui")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_UI false)
		endif()

		# uuid
		if ("${it}" STREQUAL "uuid")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_EXTRA_UUID true)
		endif()
		# -uuid
		if ("${it}" STREQUAL "-uuid")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_EXTRA_UUID false)
		endif()

		# parser
		if ("${it}" STREQUAL "parser")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_PARSER true)
		endif()
		# -parser
		if ("${it}" STREQUAL "-parser")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_PARSER false)
		endif()

		# docs
		if ("${it}" STREQUAL "docs" OR "${it}" STREQUAL "doc")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_DOCUMENTATION true)
		endif()
		# -docs
		if ("${it}" STREQUAL "-doc" OR "${it}" STREQUAL "-docs")
			set(KeywordIsKnown true)
			set(YUNI_MODULE_DOCUMENTATION false)
		endif()

		if (NOT KeywordIsKnown)
			YMESSAGE("[!!] Unknown module from command line: `${it}` (ignored)")
			set(KeywordError true)
		endif()

	endforeach()

	if (KeywordError)
		YMESSAGE("")
		YMESSAGE("Errors on modules. Here is the list of all available modules :")
		YMESSAGE("(+ : Enable the module,  - disable the module)")
		YMESSAGE(" Main and virtual modules")
		YMESSAGE("    +core          : The core module (needed)")
		YMESSAGE("    -/+tests       : Atomic Tests for the yuni framework")
		YMESSAGE("    +all           : Enable all main modules (ui,tests,...)")
		YMESSAGE("    -/+docs        : Documentation Tools")
		#YMESSAGE(" The VFS module")
		#YMESSAGE("    -/+vfs         : The Virtual filesystem")
		#YMESSAGE("    -/+vfs-local   : Support for the local filesystems")
		YMESSAGE(" The device modules")
		YMESSAGE("    -/+devices     : All devices (display,keyboard,mouse...)")
		YMESSAGE("    -/+display     : The Display device")
		YMESSAGE("    -/+keyboard    : The Keyboard device")
		YMESSAGE("    -/+mouse       : The Mouse device")
		YMESSAGE(" The media modules")
		YMESSAGE("    -/+media       : The Media module (default: disabled)")
		YMESSAGE(" The graphics modules")
		YMESSAGE("    -/+opengl      : The OpenGL renderer module (default: disabled)")
		YMESSAGE(" The ui modules")
		YMESSAGE("    -/+ui          : The ui module (default: disabled)")
		YMESSAGE(" The virtual machine module")
		YMESSAGE("    -/+vm          : The Virtual machine")
		YMESSAGE(" The network modules")
		YMESSAGE("    -/+net         : The network core module (default: disabled)")
		YMESSAGE("    -/+messaging   : The messaging module (default: disabled)")
		YMESSAGE(" The DBI modules")
		YMESSAGE("    -/+dbi         : The Database Indepedent module (default: disabled)")
		YMESSAGE(" The extra modules")
		YMESSAGE("    -/+uuid        : UUID (default: disabled)")
		YMESSAGE("    -/+marshal     : The Marshal module (for Object serialization, default: disabled)")
		YMESSAGE("    -/+algorithms  : Standard algorithms")
		YMESSAGE("    -/+parser      : Parser Generator")
		YMESSAGE("    -/+benchmarks  : Benchmarks")
		YMESSAGE("")
		message(FATAL_ERROR "Errors on module names")
	endif()
endif()


#
# Dependencies
#
if (YUNI_MODULE_OPENGL)
	set(YUNI_MODULE_GRAPHICS true)
endif()
if (YUNI_MODULE_UI)
	set(YUNI_MODULE_DEVICES true)
	set(YUNI_MODULE_DEVICE_DISPLAY true)
endif()
if (YUNI_MODULE_LDO)
	set(YUNI_MODULE_NET true)
endif()
if (YUNI_MODULE_MESSAGING)
	set(YUNI_MODULE_NET true)
	set(YUNI_MODULE_MARSHAL true)
endif()
if (YUNI_MODULE_DBI_SQLITE)
	set(YUNI_MODULE_DBI true)
endif()
if (YUNI_MODULE_DBI)
	set(YUNI_MODULE_DBI_SQLITE true)
endif()






#
# List of all available modules
#

set(YUNI_MODULE_AVAILABLE "core")

if (YUNI_MODULE_ALGORITHMS)
	list(APPEND YUNI_MODULE_AVAILABLE algorithms)
endif()

if (YUNI_MODULE_DBI)
	list(APPEND YUNI_MODULE_AVAILABLE dbi)
	if (YUNI_MODULE_DBI_SQLITE)
		list(APPEND YUNI_MODULE_AVAILABLE dbi-sqlite)
	endif()
endif()

if (YUNI_MODULE_DEVICES)
	list(APPEND YUNI_MODULE_AVAILABLE devices)
	if (YUNI_MODULE_DEVICE_DISPLAY)
		list(APPEND YUNI_MODULE_AVAILABLE display)
	endif()
	if (YUNI_MODULE_DEVICE_MOUSE)
		list(APPEND YUNI_MODULE_AVAILABLE mouse)
	endif()
	if (YUNI_MODULE_DEVICE_KEYBOARD)
		list(APPEND YUNI_MODULE_AVAILABLE keyboard)
	endif()
endif()

if (YUNI_MODULE_VFS)
	list(APPEND YUNI_MODULE_AVAILABLE vfs)
	if (YUNI_MODULE_VFS_FILE)
		list(APPEND YUNI_MODULE_AVAILABLE vfs-local)
	endif()
endif()

if (YUNI_MODULE_VM)
	list(APPEND YUNI_MODULE_AVAILABLE vm)
endif()

if (YUNI_MODULE_MARSHAL)
	list(APPEND YUNI_MODULE_AVAILABLE marshal)
endif()

if (YUNI_MODULE_NET)
	list(APPEND YUNI_MODULE_AVAILABLE net)
endif()

if (YUNI_MODULE_MESSAGING)
	list(APPEND YUNI_MODULE_AVAILABLE messaging)
endif()

if (YUNI_MODULE_LDO)
	list(APPEND YUNI_MODULE_AVAILABLE ldo)
endif()

if (YUNI_MODULE_MEDIA)
	list(APPEND YUNI_MODULE_AVAILABLE media)
endif()

if (YUNI_MODULE_GRAPHICS)
	list(APPEND YUNI_MODULE_AVAILABLE graphics)
	if (YUNI_MODULE_OPENGL)
		list(APPEND YUNI_MODULE_AVAILABLE opengl)
	endif()
endif()

if (YUNI_MODULE_UI)
	list(APPEND YUNI_MODULE_AVAILABLE ui)
endif()

if (YUNI_MODULE_DOCUMENTATION)
	list(APPEND YUNI_MODULE_AVAILABLE docs)
endif()

if (YUNI_MODULE_EXTRA_UUID)
	list(APPEND YUNI_MODULE_AVAILABLE uuid)
endif()

if (YUNI_MODULE_PARSER)
	list(APPEND YUNI_MODULE_AVAILABLE parser)
endif()

if (YUNI_BENCHMARKS)
	list(APPEND YUNI_MODULE_AVAILABLE benchmarks)
endif()

