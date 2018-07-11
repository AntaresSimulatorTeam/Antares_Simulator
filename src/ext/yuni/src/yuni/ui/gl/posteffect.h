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
#ifndef __YUNI_UI_POSTEFFECT_H__
# define __YUNI_UI_POSTEFFECT_H__

# include "../../yuni.h"
# include "../../core/smartptr.h"
# include "shadermanager.h"
# include <vector>

namespace Yuni
{
namespace UI
{


	/*!
	** \brief A post effect is a process that modifies a 2D image using shaders
	*/
	class PostEffect
	{
	public:
		typedef SmartPtr<PostEffect>  Ptr;
		typedef std::vector<Ptr>  Vector;

	public:
		PostEffect(const AnyString& name, const AnyString& vertexShader, const AnyString& fragmentShader, float width, float height):
			pName(name),
			pSize(width > 0.0f ? width : 0.0f, height > 0.0f ? height : 0.0f)
		{
			pShaders = Gfx3D::ShaderManager::Instance().getFromFiles(vertexShader, fragmentShader);
			pShaders->load();
		}

		void enableShader() const
		{
			if (!valid())
				return;
			pShaders->activate();
			pShaders->bindUniform("Texture0", Yuni::Gfx3D::Vertex<>::vaTexture0);
			pShaders->bindUniform("Width", pSize.x);
			pShaders->bindUniform("Height", pSize.y);
		}

		void disableShader() const { pShaders->deactivate(); }

		bool valid() const { return !(!pShaders) && pShaders->valid(); }

		const String& name() const { return pName; }

		void name(const AnyString& newName) { pName = newName; }

	private:
		String pName;

		Point2D<float> pSize;

		Gfx3D::ShaderProgram::Ptr pShaders;

	}; // class PostEffect



} // namespace UI
} // namespace Yuni

#endif // __YUNI_UI_POSTEFFECT_H__
