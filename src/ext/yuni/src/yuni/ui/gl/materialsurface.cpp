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
#include "materialsurface.h"
#include "texture.h"

namespace Yuni
{
namespace Gfx3D
{

	namespace // anonymous
	{

		static Texture::Ptr White = nullptr;

		static Texture::Ptr Checker = nullptr;

	} // namespace anonymous


	void MaterialSurfaceColored::activate(const ShaderProgram::Ptr& shaders, int index)
	{
		assert(shaders && shaders->valid());
		if (!White)
		{
			White = Texture::New(1, 1, 4, Texture::UInt8, nullptr, false);
			White->clearToWhite();
		}

		shaders->bindUniform(pUniformName, pColor.red, pColor.green, pColor.blue, pColor.alpha);
		// Bind the white
		if (index >= 0)
		{
			::glActiveTexture(GL_TEXTURE0 + index);
			::glBindTexture(GL_TEXTURE_2D, White->id());
			shaders->bindUniform(String("Texture") << index, Yuni::Gfx3D::Vertex<>::vaTexture0 + index);
			::glActiveTexture(GL_TEXTURE0);
		}
	}



	void MaterialSurfaceTextured::activate(const ShaderProgram::Ptr& shaders, int index)
	{
		assert(shaders && shaders->valid());

		if (index < 0)
			return;

		if (index == 0)
			shaders->bindUniform("MaterialDiffuse", 1.0f, 1.0f, 1.0f, 1.0f);
		::glActiveTexture(GL_TEXTURE0 + index);
		if (!pTexture)
		{
			// Assign a checkerboard texture to clearly show uninitialized texture materials
			if (!Checker)
				Checker = Texture::LoadFromFile("data/textures/checkerboard.jpg");
			::glBindTexture(GL_TEXTURE_2D, Checker->id());
		}
		else
			::glBindTexture(GL_TEXTURE_2D, pTexture->id());
		shaders->bindUniform(String("Texture") << index, Yuni::Gfx3D::Vertex<>::vaTexture0 + index);
		::glActiveTexture(GL_TEXTURE0);
	}



} // namespace Gfx3D
} // namespace Yuni
