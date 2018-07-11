#pragma once

/* Generate from config.h.cmake */


/*!
** \mainpage The Yuni Framework
**
** \section what What is Yuni ?
**
** The Yuni project is a high-level cross-platform framework. This framework
** intends to provide the most complete set as possible of coherent API related
** to 3D programming, especially game programming. It intends to be a reliable and
** simple bridge between different worlds to create all sorts of applications,
** letting the user focus on the real work.
**
** The Yuni project is connected with all those domains :
** - Simulation
** - Game development
** - Artificial Intelligence, mainly on 3D objects
** - Real-time and multi-user Collaboration
** - 3D graphics
** - Physics
** - Threading / Parallel computing
** - 2D/3D User Interface in 3D context
** - 2D/3D Input devices
** - Networking
** - Sound, playback of 2D/3D sounds
** - Scripting languages
** - And Any other domain related to multimedia or user interaction.
*/





/*! \name Informations about the Yuni Library */
/*@{*/
/*! The yuni website */
# define YUNI_URL_WEBSITE              "@YUNI_URL_WEBSITE@"

/*! The Devpacks repository */
# define YUNI_URL_DEVPACK_REPOSITORY   "@YUNI_URL_DEVPACK_REPOSITORY@"
/*! The Devpacks sources */
# define YUNI_URL_DEVPACK_SOURCE       "@YUNI_URL_DEVPACK_SOURCE@"

/*! The hi part of the version of the yuni library */
# define YUNI_VERSION_HI               @YUNI_VERSION_HI@
/*! The lo part of the version of the yuni library */
# define YUNI_VERSION_LO               @YUNI_VERSION_LO@
/*! The revision number */
# define YUNI_VERSION_REV              @YUNI_VERSION_REV@

/*! The complete version of Yuni */
# define YUNI_VERSION_STRING           "@YUNI_VERSION_HI@.@YUNI_VERSION_LO@.@YUNI_VERSION_REV@-@CMAKE_BUILD_TYPE@"
# define YUNI_VERSION_LITE_STRING      "@YUNI_VERSION_HI@.@YUNI_VERSION_LO@.@YUNI_VERSION_REV@"

/*! The flags used to compile Yuni */
# define YUNI_COMPILED_WITH_CXX_FLAGS  "@CMAKE_CXX_FLAGS@"
/*! The target used to compile Yuni (debug/release) */
# define YUNI_COMPILED_WITH_TARGET     "@CMAKE_BUILD_TYPE@"

/*! List of all modules */
# define YUNI_MODULE_LIST              "@YUNI_MODULE_LIST@"
/*! List of all available modules */
# define YUNI_COMPILED_WITH_MODULES    "@CMAKE_BUILD_TYPE@"

/*! Is OpenGL available ? */
# define YUNI_HAS_SUPPORT_FOR_OPENGL   @YUNI_COMPILED_WITH_SUPPORT_FOR_OPENGL@
/*! Is DirectX available ? */
# define YUNI_HAS_SUPPORT_FOR_DIRECTX  @YUNI_COMPILED_WITH_SUPPORT_FOR_DIRECTX@
/*@}*/
