
YMESSAGE_MODULE("OpenGL  (EXPERIMENTAL)")


LIBYUNI_CONFIG_LIB("both" "opengl"      "yuni-static-graphics-opengl")
LIBYUNI_CONFIG_DEPENDENCY("opengl" "core") # yuni-core is required


#
# Windows-specific
#
if (WIN32 OR WIN64)
	LIBYUNI_CONFIG_LIB("both" "opengl" opengl32 glu32 gdi32)
endif (WIN32 OR WIN64)


add_definitions("-DGLEW_STATIC")
LIBYUNI_CONFIG_CFLAG("both" "opengl" "-DGLEW_STATIC")

#
# OpenGL
#
find_package(OpenGL)
set(YUNI_HAS_OPENGL  OPENGL_FOUND)

# Found
if (YUNI_HAS_OPENGL)
	# yuni-config
	LIBYUNI_CONFIG_INCLUDE_PATH("both" "opengl" "${OPENGL_INCLUDE_DIR}")
	LIBYUNI_CONFIG_LIB("both" "opengl" "${OPENGL_LIBRARIES}")
	if (APPLE)
		# Frameworks
		LIBYUNI_CONFIG_FRAMEWORK("both" "opengl" OpenGL)
	endif ()
	# Includes
	set(CMAKE_REQUIRED_INCLUDES "${OPENGL_INCLUDE_DIR}")
	# Check everything is in order
	include(CheckIncludeFile)
	check_include_files("gl.h" YUNI_HAS_GL_H)
	check_include_files("GL/gl.h" YUNI_HAS_GL_GL_H)
	check_include_files("OpenGL/gl.h" YUNI_HAS_OPENGL_GL_H)
	if (NOT YUNI_HAS_GL_H AND NOT YUNI_HAS_GL_GL_H AND NOT YUNI_HAS_OPENGL_GL_H)
		set(YUNI_CMAKE_ERROR 1)
		YMESSAGE(    "[!!] Impossible to find gl.h. Please check your profile.")
		YMESSAGE(    " * Packages needed on Debian: libopengl-dev")
		YMESSAGE(    " * Packages needed on Ubuntu: libgl1-mesa-dev")
		YMESSAGE(    " * Packages needed on Fedora: opengl-devel")
	endif ()

# Not found
else ()
	set(YUNI_CMAKE_ERROR 1)
	YMESSAGE(    "[!!] Impossible to find OpenGL on your system. Please check your profile.")
	YMESSAGE(    " * Packages needed on Debian: libopengl-dev")
	YMESSAGE(    " * Packages needed on Ubuntu: libgl1-mesa-dev")
	YMESSAGE(    " * Packages needed on Fedora: opengl-devel")
endif ()


add_library(yuni-static-graphics-opengl STATIC
	private/graphics/opengl/glew/glew.c
	private/graphics/opengl/glew/glew.h
)

# Setting output path
set_target_properties(yuni-static-graphics-opengl PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-graphics-opengl ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

