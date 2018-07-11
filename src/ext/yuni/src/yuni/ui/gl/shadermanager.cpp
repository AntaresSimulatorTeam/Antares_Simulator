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
#include <yuni/io/filename-manipulation.h>
#include "shadermanager.h"



namespace Yuni
{
namespace Gfx3D
{


	const ShaderProgram::Ptr ShaderManager::getFromFiles(const AnyString& vShader, const AnyString& fShader)
	{
		String fullPath;
		// Use the canonicalized path as key
		Yuni::IO::Canonicalize(fullPath, vShader);

		// Search for this shader in the vertex shader cache
		VertexShader::Ptr vertex;
		auto vsIt = pVertexShaders.find(fullPath);
		if (pVertexShaders.end() != vsIt)
			vertex = vsIt->second;
		else
		{
			// Create and compile it
			vertex = new VertexShader(fullPath);
			if (!vertex->valid())
				return nullptr;
			// Add the shader to cache
			pVertexShaders[fullPath] = vertex;
		}

		// Use the canonicalized path as key
		Yuni::IO::Canonicalize(fullPath, fShader);
		// Search for this shader in the fragment shader cache
		FragmentShader::Ptr fragment;
		auto fsIt = pFragmentShaders.find(fullPath);
		if (pFragmentShaders.end() != fsIt)
			fragment = fsIt->second;
		else
		{
			// Create and compile it
			fragment = new FragmentShader(fullPath);
			if (!fragment->valid())
				return nullptr;
			// Add the shader to cache
			pFragmentShaders[fullPath] = fragment;
		}

		// Search for this pair of shaders in the shader program cache
		ShaderPair key(vertex->id(), fragment->id());
		auto progIt = pPrograms.find(key);
		if (pPrograms.end() != progIt)
			return progIt->second;

		// Add the pair to cache
		ShaderProgram::Ptr newProg = new ShaderProgram(vertex, fragment);
		pPrograms[key] = newProg;
		return newProg;
	}


	const ShaderProgram::Ptr ShaderManager::getFromMemory(const AnyString& vsCode, const AnyString& fsCode)
	{
		// Use the shader code directly as key, we can't do much better than this I'm afraid.
		VertexShader::Ptr vertex;
		auto vsIt = pVertexShaders.find(vsCode);
		if (pVertexShaders.end() != vsIt)
			vertex = vsIt->second;
		else
		{
			// Create and compile it
			vertex = new VertexShader();
			vertex->loadFromMemory(vsCode);
			if (!vertex->valid())
				return nullptr;
			// Add the shader to cache
			pVertexShaders[vsCode] = vertex;
		}

		FragmentShader::Ptr fragment;
		auto fsIt = pFragmentShaders.find(fsCode);
		if (pFragmentShaders.end() != fsIt)
			fragment = fsIt->second;
		else
		{
			// Create and compile it
			fragment = new FragmentShader();
			fragment->loadFromMemory(fsCode);
			if (!fragment->valid())
				return nullptr;
			// Add the shader to cache
			pFragmentShaders[fsCode] = fragment;
		}

		// Search for this pair of shaders in the shader program cache
		ShaderPair key(vertex->id(), fragment->id());
		auto progIt = pPrograms.find(key);
		if (pPrograms.end() != progIt)
			return progIt->second;

		// Add the pair to cache
		ShaderProgram::Ptr newProg = new ShaderProgram(vertex, fragment);
		pPrograms[key] = newProg;
		return newProg;
	}





} // namespace Gfx3D
} // namespace Yuni
