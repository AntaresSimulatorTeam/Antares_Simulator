
YMESSAGE_MODULE("UI  (EXPERIMENTAL)")

LIBYUNI_CONFIG_LIB("both" "ui"      "yuni-static-ui-core")

LIBYUNI_CONFIG_DEPENDENCY("ui" "core") # core is required
LIBYUNI_CONFIG_DEPENDENCY("ui" "ldo")
LIBYUNI_CONFIG_DEPENDENCY("ui" "messaging")
LIBYUNI_CONFIG_DEPENDENCY("ui" "net")
LIBYUNI_CONFIG_DEPENDENCY("ui" "opengl")


add_definitions("-DYUNI_MODULE_UI")

DEVPACK_IMPORT_FREETYPE()

if (NOT "${YUNI_EXT_FREETYPE_INCLUDE}" STREQUAL "")
	include_directories("${YUNI_EXT_FREETYPE_INCLUDE}")
	LIBYUNI_CONFIG_INCLUDE_PATH("both" "ui" "${YUNI_EXT_FREETYPE_INCLUDE}")
	LIBYUNI_CONFIG_LIB_PATH("both" "ui" "${YUNI_EXT_FREETYPE_LIB_PATH}")
	LIBYUNI_CONFIG_LIB("both" "ui" "${YUNI_EXT_FREETYPE_LIB}")
	LIBYUNI_CONFIG_LIB_RAW_COMMAND("both" "ui" "${YUNI_EXT_FREETYPE_LIB_RAW_COMMAND}")
endif()

list(APPEND SRC_UI
	ui/dimension.h
	ui/eventpropagation.h
	ui/font.h
	ui/ftfont.cpp
	ui/ftfont.h
	ui/multisampling.h
	ui/pictureoverlay.cpp
	ui/pictureoverlay.h
	ui/pictureoverlay.hxx
	ui/renderwindow.cpp
	ui/renderwindow.h
	ui/textoverlay.cpp
	ui/textoverlay.h
	ui/textoverlay.hxx
	ui/theme.cpp
	ui/theme.h
	ui/theme.hxx
	ui/windowfactory.cpp
	ui/windowfactory.h

	# Components
	ui/control/button.cpp
	ui/control/button.h
	ui/control/control.cpp
	ui/control/control.h
	ui/control/checkbox.cpp
	ui/control/checkbox.h
	ui/control/listbox.cpp
	ui/control/listbox.h
	ui/control/listbox.hxx
	ui/control/panel.cpp
	ui/control/panel.h
	ui/control/picture.cpp
	ui/control/picture.h
	ui/control/splitter.cpp
	ui/control/splitter.h
	ui/control/text.cpp
	ui/control/text.h
	ui/control/texteditor.cpp
	ui/control/texteditor.h
	ui/control/window.cpp
	ui/control/window.h

	# Input
	ui/input/key.h
	ui/input/keyboard.h
	ui/input/mouse.cpp
	ui/input/mouse.h
	ui/input/winmouse.h

	# Image loading
	ui/stb_image.cpp
	ui/stb_image.h

	# Animation
	ui/animation/apply.h
	ui/animation/easing.h
	ui/animation/transition.h
	ui/animation/transition.hxx
)

list(APPEND SRC_UI_GL
	# OpenGL-specific
	ui/gl/drawingsurface.cpp
	ui/gl/drawingsurface.h
	ui/gl/framebuffer.cpp
	ui/gl/framebuffer.h
	ui/gl/framebuffer.hxx
	ui/gl/glerror.h
	ui/gl/glwindow.cpp
	ui/gl/glwindow.h
	ui/gl/material.cpp
	ui/gl/material.h
	ui/gl/material.hxx
	ui/gl/materialsurface.cpp
	ui/gl/materialsurface.h
	ui/gl/shader.cpp
	ui/gl/shader.h
	ui/gl/shader.hxx
	ui/gl/shadermanager.cpp
	ui/gl/shadermanager.h
	ui/gl/shaderprogram.cpp
	ui/gl/shaderprogram.h
	ui/gl/shaderprogram.hxx
	ui/gl/shaders.cpp
	ui/gl/shaders.h
	ui/gl/texture.cpp
	ui/gl/texture.h
	ui/gl/vertex.h
	ui/gl/vertex.hxx
	ui/gl/vertexpadding.h
	ui/gl/vertexpolicies.h
	ui/gl/view.cpp
	ui/gl/view.h
	ui/gl/linerenderer.h
	ui/gl/linerenderer.cpp
)

if (YUNI_HAS_OPENGL)
   list(APPEND SRC_UI "${SRC_UI_GL}")
	#
	# System-dependent
	#
	if (WIN32 OR WIN64)
		list(APPEND SRC_UI ui/gl/wglwindow.h ui/gl/wglwindow.cpp)
	else ()
		if (APPLE)
			list(APPEND SRC_UI ui/gl/nsglwindow.h ui/gl/nsglwindow.cpp ui/gl/CMacsTypes.h ui/gl/CMacsTypes.cpp)
			set(YUNI_OPENGL_LIB "-framework OpenGL -framework AGL -framework AppKit -framework Foundation")
		else ()
			if (UNIX)
				list (APPEND SRC_UI ui/gl/glxwindow.h ui/gl/glxwindow.hxx ui/gl/glxwindow.cpp)
			endif ()
		endif ()
	endif ()
else()
   YERROR("UI module currently requires OpenGL to work !")
endif()

add_library(yuni-static-ui-core STATIC ${YUNI_EXT_FREETYPE_INCLUDE} ${SRC_UI})
target_link_libraries(yuni-static-ui-core ${YUNI_EXT_FREETYPE_LIB} ${YUNI_OPENGL_LIB})

