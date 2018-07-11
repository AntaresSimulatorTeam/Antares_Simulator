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
#ifndef __YUNI_UI_GL_SHADERPROGRAM_HXX__
# define __YUNI_UI_GL_SHADERPROGRAM_HXX__

# include "../../core/static/assert.h"
# include "glerror.h"


namespace Yuni
{
namespace Gfx3D
{

	inline bool ShaderProgram::valid() const
	{
		return pLinked;
	}


	inline void ShaderProgram::bindUniform(const AnyString& name, Vertex<>::Attribute value) const
	{
		// int, not uint, otherwise using Texture[0-7] will not work...
		// From the man page : "glUniform1i and glUniform1iv are the only two functions
		// that may be used to load uniform variables defined as sampler types"
		bindUniform(name, static_cast<int>(value));
	}


	inline const String& ShaderProgram::errorMessage() const
	{
		return pError;
	}


	inline void ShaderProgram::vertexShader(const VertexShader::Ptr& shader)
	{
		pVertexShader = shader;
	}


	inline const VertexShader::Ptr& ShaderProgram::vertexShader() const
	{
		return pVertexShader;
	}


	inline void ShaderProgram::fragmentShader(const FragmentShader::Ptr& shader)
	{
		pFragmentShader = shader;
	}


	inline const FragmentShader::Ptr& ShaderProgram::fragmentShader() const
	{
		return pFragmentShader;
	}


	inline void ShaderProgram::computeShader(const ComputeShader::Ptr& shader)
	{
		pComputeShader = shader;
	}


	inline const ComputeShader::Ptr& ShaderProgram::computeShader() const
	{
		return pComputeShader;
	}





} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_UI_GL_SHADERPROGRAM_HXX__
