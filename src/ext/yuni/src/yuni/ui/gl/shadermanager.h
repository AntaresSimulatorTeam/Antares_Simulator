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
#ifndef __YUNI_UI_GL_SHADERMANAGER_H__
# define __YUNI_UI_GL_SHADERMANAGER_H__

# include <yuni/yuni.h>
# include <yuni/core/dictionary.h>
# include <yuni/core/singleton.h>
# include "shaderprogram.h"

namespace Yuni
{
namespace Gfx3D
{


	/*!
	** \brief Create shader programs and shaders
	*/
	class ShaderManager : public Singleton<ShaderManager>
	{
	public:
		//! Pair of vertex + fragment shader IDs
		typedef std::pair<IShader::ID, IShader::ID>  ShaderPair;

		//! Cache for shader programs, by vertex + fragment shader IDs
		typedef Dictionary<ShaderPair, ShaderProgram::Ptr>::Ordered ProgramCache;


	public:
		/*!
		** \brief Get a shader program from the paths to the vertex and fragment shaders
		**
		** \returns The shader program if both shaders were successfully loaded. nullptr otherwise.
		** \warning The attributes need to be binded after this, and load() must be called.
		** \code
		** bool loadShaders(ShaderProgram::Ptr& program)
		** {
		**   program = ShaderManager::getFromFiles("vertexshader.glsl", "fragmentshader.glsl");
		**   if (!program)
		**   {
		**     std::cerr << "Could not find shaders !" << std::endl;
		**     return false;
		**   }
		**
		**   // Bind attributes
		**   program->bindAttribute("MyTexture", Vertex<>::vaTexture0);
		**   // Link the program
		**   if (!program->load())
		**   {
		**     std::cerr << "Could not link shader program !" << std::endl;
		**     return false;
		**   }
		**
		**   // Activate before binding uniforms
		**   program->activate();
		**   // Bind uniforms
		**   program->bindUniform("MyFloatValue", 0.0f);
		**   // You may deactivate it here if you do not want to draw with it immediately
		**   program->deactivate();
		**   return true;
		** }
		** \endcode
		*/
		const ShaderProgram::Ptr getFromFiles(const AnyString& vsPath, const AnyString& fsPath);

		/*
		** \brief Get a shader program from strings containing the vertex and fragment shaders
		**
		** \returns The shader program if both shaders were successfully loaded. nullptr otherwise.
		** \warning The attributes need to be binded after this, and load() must be called.
		*/
		const ShaderProgram::Ptr getFromMemory(const AnyString& vsCode, const AnyString& fsCode);


	private:
		//! Private constructor
		ShaderManager() {}

		//! Private copy constructor
		ShaderManager(const ShaderManager&);

	private:
		//! Current programs as referenced by the IDs of the shaders they contain
		ProgramCache pPrograms;
		//! Currently loaded vertex shaders
		VertexShader::Map pVertexShaders;
		//! Currently loaded fragment shaders
		FragmentShader::Map pFragmentShaders;

		//! Friend declaration for constructor access
		template<class T> friend class Yuni::Policy::Creation::EmptyConstructor;

	}; // class ShaderManager





} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_UI_GL_SHADERMANAGER_H__
