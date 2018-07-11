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
#ifndef __YUNI_UI_GL_SHADER_H__
# define __YUNI_UI_GL_SHADER_H__

# include <yuni/yuni.h>
# include <yuni/core/dictionary.h>
# include <yuni/core/smartptr.h>
# include <yuni/core/string.h>
# include <yuni/core/noncopyable.h>



namespace Yuni
{
namespace Gfx3D
{

	//! Forward declaration
	class ShaderProgram;


	/*!
	** \brief Shader loading
	*/
	class IShader : public NonCopyable<IShader>
	{
	public:
		//! ID type
		typedef uint  ID;
		enum
		{
			invalidID = (uint) -1,
		};

	public:
		//! Constructor
		IShader();
		# ifdef YUNI_HAS_CPP_MOVE
		IShader(IShader&& rhs);
		# endif
		//! Virtual destructor
		virtual ~IShader() {}

		//! Load the source from file
		virtual bool loadFromFile(const AnyString& filePath) = 0;

		//! Load the source from memory
		virtual bool loadFromMemory(const AnyString& source) = 0;

		//! Is the shader valid ?
		bool valid() const;

		//! Get the ID
		uint id() const;

	protected:
		uint pID;

		//! Friend declaration : ShaderProgram
		friend class ShaderProgram;

	}; // class IShader





	/*!
	** \brief Vertex shader loading
	*/
	class VertexShader final : public IShader
	{
	public:
		// Smart pointer
		typedef SmartPtr<VertexShader>  Ptr;

		//! Dictionary
		typedef Dictionary<String, Ptr>::Hash  Map;

	public:
		//! Constructor
		VertexShader() {}
		VertexShader(const AnyString& filePath);
		//! Virtual destructor
		virtual ~VertexShader();

		virtual bool loadFromFile(const AnyString& filePath) override;
		virtual bool loadFromMemory(const AnyString& source) override;

	}; // class VertexShader


	/*!
	** \brief Fragment shader loading
	*/
	class FragmentShader final : public IShader
	{
	public:
		// Smart pointer
		typedef SmartPtr<FragmentShader>  Ptr;

		//! Dictionary
		typedef Dictionary<String, Ptr>::Hash  Map;

	public:
		//! Constructor
		FragmentShader() {}
		FragmentShader(const AnyString& filePath);
		//! Virtual destructor
		virtual ~FragmentShader();

		virtual bool loadFromFile(const AnyString& filePath) override;
		virtual bool loadFromMemory(const AnyString& source) override;

	}; // class FragmentShader


	/*!
	** \brief Compute shader loading
	*/
	class ComputeShader final : public IShader
	{
	public:
		// Smart pointer
		typedef SmartPtr<ComputeShader>  Ptr;

	public:
		//! Constructor
		ComputeShader() {}
		ComputeShader(const AnyString& filePath);
		//! Virtual destructor
		virtual ~ComputeShader();

		virtual bool loadFromFile(const AnyString& filePath) override;
		virtual bool loadFromMemory(const AnyString& source) override;

	}; // class ComputeShader






} // namespace Gfx3D
} // namespace Yuni


#include "shader.hxx"


#endif // __YUNI_UI_GL_SHADER_H__
