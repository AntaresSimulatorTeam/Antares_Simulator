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
#ifndef __YUNI_UI_GL_SHADERPROGRAM_H__
# define __YUNI_UI_GL_SHADERPROGRAM_H__

# include "../../yuni.h"
# include "../../core/smartptr.h"
# include "shader.h"
# include "texture.h"
# include "vertex.h"
# include <map>


namespace Yuni
{
namespace Gfx3D
{

	/*!
	** \brief A shader program can be bound to a vertex shader, a fragment shader and a compute shader
	*/
	class ShaderProgram final
	{
	public:
		//! Smart pointer
		typedef SmartPtr<ShaderProgram>  Ptr;
		//! Cache for name locations in the shader program
		typedef std::map<String, int>  LocationCache;

	public:
		//! \name Constructors & Destructor
		//@{
		//! Constructor
		ShaderProgram();
		//! Constructor with the shaders already compiled
		ShaderProgram(const VertexShader::Ptr& vShader, const FragmentShader::Ptr& fShader);
		//! Constructor with the paths to the shaders
		ShaderProgram(const AnyString& vShaderPath, const AnyString& fShaderPath);
		//! Destructor
		~ShaderProgram();
		//@}

		//! Set the vertex shader
		void vertexShader(const VertexShader::Ptr& shader);
		//! Get the vertex shader
		const VertexShader::Ptr& vertexShader() const;

		//! Set the fragment shader
		void fragmentShader(const FragmentShader::Ptr& shader);
		//! Get the fragment shader
		const FragmentShader::Ptr& fragmentShader() const;

		//! Set the compute shader
		void computeShader(const ComputeShader::Ptr& shader);
		//! Get the compute shader
		const ComputeShader::Ptr& computeShader() const;

		//! Load the program
		bool load();

		//! Activate this program
		void activate() const;

		/*!
		** 'brief Deactivate this program
		**
		** \internal This method does not use any class data in the current
		**   implementation but it may change in the future
		*/
		static void deactivate();

		//! Is the program valid for use ?
		bool valid() const;

		//! Get the error message, empty string if none
		const String& errorMessage() const;

		/*!
		** \brief Bind an attribute index to a variable name in the shader program
		**
		** \warning This must be called before the program is linked !
		*/
		void bindAttribute(const AnyString& name, Vertex<>::Attribute attribIndex) const;

		/*!
		** \brief  Bind a uniform value to a name in the shader program
		**
		** \warning This must be called after the program is linked and activated !
		*/
		void bindUniform(const AnyString& name, int value) const;
		void bindUniform(const AnyString& name, uint value) const;
		void bindUniform(const AnyString& name, float value) const;
		void bindUniform(const AnyString& name, Vertex<>::Attribute value) const;
		void bindUniform(const AnyString& name, const Vector3D<int>& value) const;
		void bindUniform(const AnyString& name, const Vector3D<float>& value) const;
		void bindUniform(const AnyString& name, const Vector3D<uint>& value) const;
		void bindUniform(const AnyString& name, const Color::RGB<float>& value) const;
		void bindUniform(const AnyString& name, const Color::RGBA<float>& value) const;
		// Temporary prototype, we need a Vector4 or matrices
		void bindUniform(const AnyString& name, float v1, float v2, float v3, float v4) const;

		//! Bind a uniform array of values to a name in the shader program
		void bindUniformArray(const AnyString& name, uint count, uint components, const int* array) const;
		void bindUniformArray(const AnyString& name, uint count, uint components, const uint* array) const;
		void bindUniformArray(const AnyString& name, uint count, uint components, const float* array) const;

		//! Bind a texture as an image for compute shader use
		void bindImage(const AnyString& name, const Texture::Ptr& texture,
			Vertex<>::Attribute value) const;

	private:
		//! ID of the shader program
		mutable uint pID;
		//! Is the program already linked ?
		mutable bool pLinked;

		//! Current vertex shader
		VertexShader::Ptr pVertexShader;
		//! Current fragment shader
		FragmentShader::Ptr pFragmentShader;
		//! Current compute shader
		ComputeShader::Ptr pComputeShader;

		//! Error string storage
		String pError;
		//! Cache for uniform locations
		mutable LocationCache pUniformCache;

	}; // class ShaderProgram





} // namespace Gfx3D
} // namespace Yuni

# include "shaderprogram.hxx"

#endif // __YUNI_UI_GL_SHADERPROGRAM_H__
