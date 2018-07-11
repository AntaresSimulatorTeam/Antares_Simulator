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
#include "material.h"
#include "shaderprogram.h"

namespace Yuni
{
namespace Gfx3D
{

	namespace // anonymous
	{
		Material::Ptr sDefault = new Material();

	}; // namespace anonymous


	const Material::Ptr& Material::Default()
	{
		return sDefault;
	}


	void Material::activate(const ShaderProgram::Ptr& shaders)
	{
		if (!shaders || !shaders->valid())
			return;

		pAmbient->activate(shaders, -1);
		pSpecular->activate(shaders, 1);
		pEmission->activate(shaders, 2);
		pDiffuse->activate(shaders, 0);
		// TODO : Have a normal map also ? Is this really material-related ?
	}



} // namespace Gfx3D
} // namespace Yuni
