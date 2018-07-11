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
#ifndef __YUNI_GFX3D_MATERIALSURFACE_H__
# define __YUNI_GFX3D_MATERIALSURFACE_H__

# include <yuni/yuni.h>
# include <yuni/core/smartptr.h>
# include <yuni/core/string.h>
# include <yuni/core/color/rgba.h>
# include <vector>
# include "texture.h"
# include "shaderprogram.h"

namespace Yuni
{
namespace Gfx3D
{


	/*!
	** \name A material surface is an abstract base for various ways of coloring in a material
	*/
	class MaterialSurface
	{
	public:
		//! Smart pointer
		typedef SmartPtr<MaterialSurface>  Ptr;

	public:
		//! Virtual destructor
		virtual ~MaterialSurface() {}

		//! Activate this surface
		virtual void activate(const ShaderProgram::Ptr& shaders, int index) = 0;

	}; // class MaterialSurface


	/*!
	** \name Textured surface
	*/
	class MaterialSurfaceTextured: public MaterialSurface
	{
	public:
		//! Constructor
		MaterialSurfaceTextured(const Texture::Ptr& texture):
			pTexture(texture)
		{}

		//! Virtual destructor
		virtual ~MaterialSurfaceTextured() {}

		//! Activate this surface
		virtual void activate(const ShaderProgram::Ptr& shaders, int index) override;

		//! Texture used on this surface
		const Texture::Ptr& texture() const { return pTexture; }

	private:
		//! Texture used on this surface
		Texture::Ptr pTexture;

	}; // class MaterialSurfaceTextured


	/*!
	** \name Singly-colored material surface
	*/
	class MaterialSurfaceColored: public MaterialSurface
	{
	public:
		template<class T>
		MaterialSurfaceColored(const AnyString& uniformName, T r, T g, T b):
			pUniformName(uniformName),
			pColor(r, g, b)
		{}

		template<class T>
		MaterialSurfaceColored(const AnyString& uniformName, T r, T g, T b, T a):
			pUniformName(uniformName),
			pColor(r, g, b, a)
		{}

		template<class T>
		MaterialSurfaceColored(const AnyString& uniformName, const Color::RGB<T>& color):
			pUniformName(uniformName),
			pColor(color)
		{}

		template<class T>
		MaterialSurfaceColored(const AnyString& uniformName, const Color::RGBA<T>& color):
			pUniformName(uniformName),
			pColor(color)
		{}

		//! Virtual destructor
		virtual ~MaterialSurfaceColored() {}

		//! Activate this surface
		virtual void activate(const ShaderProgram::Ptr& shaders, int index) override;

		//! Surface color
		const Color::RGBA<float>& color() const { return pColor; }

	private:
		//! Name of the corresponding uniform in the shaders
		String pUniformName;

		//! Color used for this surface
		Color::RGBA<float> pColor;

	}; // class MaterialSurfaceColored



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_MATERIALSURFACE_H__
