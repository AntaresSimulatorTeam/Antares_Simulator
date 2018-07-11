/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_GFX3D_GL_ERROR_H__
# define __YUNI_GFX3D_GL_ERROR_H__

#	include <iostream>
#	include <yuni/core/logs.h>
#	include <yuni/core/string.h>
#	include <yuni/private/graphics/opengl/glew/glew.h>

namespace Yuni
{
namespace Gfx3D
{

	inline void GLClearError()
	{
		# ifndef NDEBUG
		::glGetError();
		# endif // DEBUG
	}


# ifndef NDEBUG
	inline bool GLTestError(const AnyString& location)
	{

		GLenum err = ::glGetError();
		if (err == GL_NO_ERROR)
			return true;

		const char* errorText;

		switch (err)
		{
			case GL_NO_ERROR:
				errorText = "GL_NO_ERROR";
				break;
			case GL_INVALID_VALUE:
				errorText = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_ENUM:
				errorText = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_OPERATION:
				errorText = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				errorText = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				errorText = "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				errorText = "GL_OUT_OF_MEMORY";
				break;
			default:
				errorText = "Unknown GL error";
		}

		std::cerr << errorText << " in " << location << std::endl;
		return false;
	}
# else
	inline bool GLTestError(const AnyString&)
	{
		return true;
	}
# endif // !NDEBUG



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_GL_ERROR_H__
