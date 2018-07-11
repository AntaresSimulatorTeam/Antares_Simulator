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
#ifndef __YUNI_GFX3D_MATERIAL_H__
# define __YUNI_GFX3D_MATERIAL_H__

# include <yuni/yuni.h>
# include <yuni/string.h>
# include <yuni/core/smartptr.h>
# include <yuni/core/color/rgb.h>
# include <yuni/core/color/rgba.h>
# include <yuni/uuid/uuid.h>
# include <vector>
# include <map>
# include "texture.h"
# include "materialsurface.h"

namespace Yuni
{
namespace Gfx3D
{


	/*!
	** \name A material defines how a 3D surface is textured and how it treats lighting
	*/
	class Material
	{
	public:
		//! Smart Pointer
		typedef SmartPtr<Material>  Ptr;
		//! ID
		typedef UUID  ID;
		//! Vector of smart pointers
		typedef std::vector<Ptr>  Vector;
		//! Map ID -> Ptr
		typedef std::map<ID, Ptr>  Map;

	public:
		//! Default material : colored surface, opaque white
		static const Material::Ptr& Default();

	public:
		//! Constructor
		Material();

		//! Get the material ID
		const ID& id() const;

		//! Get the name of the material
		const String& name() const;
		//! Set the name of the material
		void name(const AnyString& name);

		//! Activate this material for drawing
		void activate(const ShaderProgram::Ptr& shaders);

		//! Get the ambient color
		const MaterialSurface::Ptr& ambient() const;
		//! Set the ambient to an RGB color (full opacity)
		template <class T>
		void ambient(const Color::RGB<T>& ambient);
		//! Set the ambient to an RGBA color
		template <class T>
		void ambient(const Color::RGBA<T>& ambient);
		//! Set the ambient to a texture
		void ambient(const Texture::Ptr& texture);

		//! Get the diffuse color
		const MaterialSurface::Ptr& diffuse() const;
		//! Set the diffuse to an RGB color (full opacity)
		template <class T>
		void diffuse(const Color::RGB<T>& diffuse);
		//! Set the diffuse to an RGBA color
		template <class T>
		void diffuse(const Color::RGBA<T>& diffuse);
		//! Set the diffuse to a texture
		void diffuse(const Texture::Ptr& texture);

		//! Get the specular color
		const MaterialSurface::Ptr& specular() const;
		//! Set the specular to an RGB color (full opacity)
		template <class T>
		void specular(const Color::RGB<T>& specular);
		//! Set the specular to an RGBA color
		template <class T>
		void specular(const Color::RGBA<T>& specular);
		//! Set the specular to a texture
		void specular(const Texture::Ptr& texture);

		//! Get the emission color
		const MaterialSurface::Ptr& emission() const;
		//! Set the emission to an RGB color (full opacity)
		template <class T>
		void emission(const Color::RGB<T>& emission);
		//! Set the emission to an RGBA color
		template <class T>
		void emission(const Color::RGBA<T>& emission);
		//! Set the emission to a texture
		void emission(const Texture::Ptr& texture);

		//! Get the shininess constant
		float shininess() const;
		//! Set the shininess constant
		void shininess(float shininess);

		//! Get the transparency
		float transparency() const;
		//! Set the transparency
		void transparency(float transparency);

		//! Get the refraction index
		float refractionIndex() const;
		//! Set the refraction index
		void refractionIndex(float newIndex);

	private:
		//! Material name
		String pName;

		//! Unique ID
		ID pID;

		MaterialSurface::Ptr pAmbient;

		MaterialSurface::Ptr pDiffuse;

		MaterialSurface::Ptr pSpecular;

		MaterialSurface::Ptr pEmission;

		float pShininess;

		float pTransparency;

		float pRefractionIndex;

	}; // class Material



} // namespace Gfx3D
} // namespace Yuni

# include "material.hxx"

#endif // __YUNI_GFX3D_MATERIAL_H__
