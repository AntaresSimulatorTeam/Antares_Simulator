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
#include "shaderprogram.h"
#include "../../private/graphics/opengl/glew/glew.h"


namespace Yuni
{
namespace Gfx3D
{

	static inline int GetUniformLocation(uint id, ShaderProgram::LocationCache& cache, const AnyString& name)
	{
		GLClearError();
		ShaderProgram::LocationCache::const_iterator it = cache.find(name);
		if (cache.end() != it)
			return it->second;

		int location = ::glGetUniformLocation(id, name.c_str());
		if (not GLTestError("ShaderProgram::bindUniform, glGetUniformLocation"))
			return -1;

		cache[name] = location;
		return location;
	}





	ShaderProgram::ShaderProgram():
		pID(::glCreateProgram()),
		pLinked(false)
	{}


	ShaderProgram::ShaderProgram(const VertexShader::Ptr& vShader, const FragmentShader::Ptr& fShader):
		pID(::glCreateProgram()),
		pLinked(false),
		pVertexShader(vShader),
		pFragmentShader(fShader),
		pComputeShader(nullptr)
	{}


	ShaderProgram::ShaderProgram(const AnyString& vShaderPath, const AnyString& fShaderPath):
		pID(::glCreateProgram()),
		pLinked(false)
	{
		VertexShader* vShader = new VertexShader();
		if (not vShader->loadFromFile(vShaderPath))
		{
			delete vShader;
			return;
		}
		pVertexShader = vShader;

		FragmentShader* fShader = new FragmentShader();
		if (not fShader->loadFromFile(fShaderPath))
		{
			delete fShader;
			return;
		}
		pFragmentShader = fShader;
	}


	ShaderProgram::~ShaderProgram()
	{
		if (pVertexShader)
		{
			::glDetachShader(pID, pVertexShader->pID);
			pVertexShader = nullptr; // release shader
		}

		if (pFragmentShader)
		{
			::glDetachShader(pID, pFragmentShader->pID);
			pFragmentShader = nullptr; // release shader
		}

		if (pComputeShader)
		{
			::glDetachShader(pID, pComputeShader->pID);
			pComputeShader = nullptr; // release shader
		}

		::glDeleteProgram(pID);
	}


	bool ShaderProgram::load()
	{
		GLClearError();
		pError.clear();

		// If not yet linked, do it on-the-fly
		if (not pLinked)
		{
			// Make sure we have at least one of the three shaders
			if ((!pVertexShader or 0 == pVertexShader->pID)
				and (!pFragmentShader or 0 == pFragmentShader->pID)
				and (!pComputeShader or 0 == pComputeShader->pID))
			{
				return false;
			}

			if (!(!pVertexShader) and pVertexShader->pID)
			{
				::glAttachShader(pID, pVertexShader->pID);
				GLTestError("ShaderProgram::load, glAttachShader, attaching vertex shader");
			}

			if (!(!pFragmentShader) and pFragmentShader->pID)
			{
				::glAttachShader(pID, pFragmentShader->pID);
				GLTestError("ShaderProgram::load, glAttachShader, attaching fragment shader");
			}

			if (!(!pComputeShader) and pComputeShader->pID)
			{
				::glAttachShader(pID, pComputeShader->pID);
				GLTestError("ShaderProgram::load, glAttachShader, attaching compute shader");
			}

			// Link
			::glLinkProgram(pID);
			GLTestError("ShaderProgram::load, glLinkProgram");

			int linked;
			::glGetProgramiv(pID, GL_LINK_STATUS, &linked);
			GLTestError("ShaderProgram::load, glGetProgramiv");

			pLinked = (linked == GL_TRUE);
			if (0 == pLinked)
			{
				char log[1024];
				int length = 0;
				::glGetProgramInfoLog(pID, 1024, &length, log);

				pError.clear() << "Shader program " << pID << " failed to load !";
				if ((uint) length < 1024)
				{
					log[(uint)length] = '\0'; // for safety, just in case
					pError += '\n';
					pError.append(log, (uint)length);
				}
				return false;
			}
		}
		return true;
	}


	void ShaderProgram::activate() const
	{
		GLClearError();
		::glUseProgram(pID);
		GLTestError("ShaderProgram::activate, glUseProgram");
	}


	void ShaderProgram::deactivate()
	{
		::glUseProgram(0);
	}


	void ShaderProgram::bindAttribute(const AnyString& name, Vertex<>::Attribute attribIndex) const
	{
		GLClearError();
		::glBindAttribLocation(pID, attribIndex, name.c_str());
		GLTestError("ShaderProgram::bindAttribute, glBindAttribLocation");
	}


	void ShaderProgram::bindUniform(const AnyString& name, int value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
		{
			::glUniform1i(location, value);
			GLTestError("ShaderProgram::bindUniform, glUniform1i");
		}
	}

	void ShaderProgram::bindUniform(const AnyString& name, float value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform1f(location, value);
	}

	void ShaderProgram::bindUniform(const AnyString& name, uint value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform1ui(location, value);
	}


	void ShaderProgram::bindUniform(const AnyString& name, const Vector3D<int>& value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform3i(location, value.x, value.y, value.z);
	}


	void ShaderProgram::bindUniform(const AnyString& name, const Vector3D<float>& value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform3f(location, value.x, value.y, value.z);
	}


	void ShaderProgram::bindUniform(const AnyString& name, const Vector3D<uint>& value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform3ui(location, value.x, value.y, value.z);
	}


	void ShaderProgram::bindUniform(const AnyString& name, const Color::RGB<float>& value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			return;
		::glUniform3f(location, value.red, value.green, value.blue);
	}


	void ShaderProgram::bindUniform(const AnyString& name, const Color::RGBA<float>& value) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform4f(location, value.red, value.green, value.blue, value.alpha);
	}


	void ShaderProgram::bindUniform(const AnyString& name, float v1, float v2, float v3, float v4) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location >= 0)
			::glUniform4f(location, v1, v2, v3, v4);
	}


	void ShaderProgram::bindUniformArray(const AnyString& name, uint count, uint components,
		const int* array) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location < 0)
			return;

		switch (components)
		{
			case 1:
				::glUniform1iv(location, (int) count, array);
				break;
			case 2:
				::glUniform2iv(location, (int) count, array);
				break;
			case 3:
				::glUniform3iv(location, (int) count, array);
				break;
			case 4:
				::glUniform4iv(location, (int) count, array);
				break;
			default:
				assert(false and "Invalid number of components : must be 1-4.");
		}
	}


	void ShaderProgram::bindUniformArray(const AnyString& name, uint count, uint components,
		const float* array) const
	{
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location < 0)
			return;

		switch (components)
		{
			case 1:
				::glUniform1fv(location, (int) count, array);
				break;
			case 2:
				::glUniform2fv(location, (int) count, array);
				break;
			case 3:
				::glUniform3fv(location, (int) count, array);
				break;
			case 4:
				::glUniform4fv(location, (int) count, array);
				break;
			default:
				assert(false and "Invalid number of components : must be 1-4.");
		}
	}


	void ShaderProgram::bindUniformArray(const AnyString& name, uint count, uint components,
		const uint* array) const
	{
		assert(components >= 1 and components <= 4 and "Invalid number of components : must be 1-4.");
		GLint location = GetUniformLocation(pID, pUniformCache, name);
		if (location < 0)
			return;

		switch (components)
		{
			case 1:
				::glUniform1uiv(location, (int) count, array);
				break;
			case 2:
				::glUniform2uiv(location, (int) count, array);
				break;
			case 3:
				::glUniform3uiv(location, (int) count, array);
				break;
			case 4:
				::glUniform4uiv(location, (int) count, array);
				break;
			default:
				assert(false and "Invalid number of components : must be 1-4.");
		}
	}



	void ShaderProgram::bindImage(const AnyString& name, const Texture::Ptr& texture,
		Vertex<>::Attribute value) const
	{
		GLClearError();
		::glBindTexture(GL_TEXTURE_2D, texture->id());
		::glBindImageTexture(0, texture->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
		if (not GLTestError("ShaderProgram::bindImage, glBindImageTexture"))
			return;
		bindUniform(name, value);
	}




} // namespace Gfx3D
} // namespace Yuni
