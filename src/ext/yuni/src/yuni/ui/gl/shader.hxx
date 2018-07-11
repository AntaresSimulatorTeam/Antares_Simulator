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
#ifndef __YUNI_UI_GL_SHADER_HXX__
# define __YUNI_UI_GL_SHADER_HXX__



namespace Yuni
{
namespace Gfx3D
{


	inline IShader::IShader()
		: pID(invalidID)
	{}


	# ifdef YUNI_HAS_CPP_MOVE
	inline IShader::IShader(IShader&& rhs)
		: pID(rhs.pID)
	{
		rhs.pID = invalidID;
	}
	# endif


	inline bool IShader::valid() const
	{
		return pID != invalidID;
	}


	inline uint IShader::id() const
	{
		return pID;
	}






} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_UI_GL_SHADER_HXX__
