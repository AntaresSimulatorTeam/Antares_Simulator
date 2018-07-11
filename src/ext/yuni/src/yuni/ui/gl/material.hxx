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
#ifndef __YUNI_GFX3D_MATERIAL_HXX__
# define __YUNI_GFX3D_MATERIAL_HXX__

namespace Yuni
{
namespace Gfx3D
{


	inline Material::Material():
		pName("New material"),
		pID(UUID::fGenerate),
		pAmbient(new MaterialSurfaceColored("MaterialAmbient", 1.0f, 1.0f, 1.0f)),
		pDiffuse(new MaterialSurfaceColored("MaterialDiffuse", 1.0f, 1.0f, 1.0f)),
		pSpecular(new MaterialSurfaceColored("MaterialSpecular", 0.0f, 0.0f, 0.0f)),
		pEmission(new MaterialSurfaceColored("MaterialEmission", 0.0f, 0.0f, 0.0f)),
		pShininess(1.0f),
		pTransparency(0.0f),
		pRefractionIndex(1.0f)
	{}


	inline const Material::ID& Material::id() const
	{
		return pID;
	}


	inline const String& Material::name() const
	{
		return pName;
	}

	inline void Material::name(const AnyString& name)
	{
		pName = name;
	}



	inline const MaterialSurface::Ptr& Material::ambient() const
	{
		return pAmbient;
	}

	template <class T>
	inline void Material::ambient(const Color::RGB<T>& ambient)
	{
		pAmbient = new MaterialSurfaceColored("MaterialAmbient", ambient);
	}

	template <class T>
	inline void Material::ambient(const Color::RGBA<T>& ambient)
	{
		pAmbient = new MaterialSurfaceColored("MaterialAmbient", ambient);
	}


	inline void Material::ambient(const Texture::Ptr& texture)
	{
		pAmbient = new MaterialSurfaceTextured(texture);
	}



	inline const MaterialSurface::Ptr& Material::diffuse() const
	{
		return pDiffuse;
	}


	template <class T>
	inline void Material::diffuse(const Color::RGB<T>& diffuse)
	{
		pDiffuse = new MaterialSurfaceColored("MaterialDiffuse", diffuse);
	}


	template <class T>
	inline void Material::diffuse(const Color::RGBA<T>& diffuse)
	{
		pDiffuse = new MaterialSurfaceColored("MaterialDiffuse", diffuse);
	}


	inline void Material::diffuse(const Texture::Ptr& texture)
	{
		pDiffuse = new MaterialSurfaceTextured(texture);
	}



	inline const MaterialSurface::Ptr& Material::specular() const
	{
		return pSpecular;
	}


	template <class T>
	inline void Material::specular(const Color::RGB<T>& specular)
	{
		pSpecular = new MaterialSurfaceColored("MaterialSpecular", specular);
	}


	template <class T>
	inline void Material::specular(const Color::RGBA<T>& specular)
	{
		pSpecular = new MaterialSurfaceColored("MaterialSpecular", specular);
	}


	inline void Material::specular(const Texture::Ptr& texture)
	{
		pSpecular = new MaterialSurfaceTextured(texture);
	}



	inline const MaterialSurface::Ptr& Material::emission() const
	{
		return pEmission;
	}


	template <class T>
	inline void Material::emission(const Color::RGB<T>& emission)
	{
		pEmission = new MaterialSurfaceColored("MaterialEmission", emission);
	}


	template <class T>
	inline void Material::emission(const Color::RGBA<T>& emission)
	{
		pEmission = new MaterialSurfaceColored("MaterialEmission", emission);
	}


	inline void Material::emission(const Texture::Ptr& texture)
	{
		pEmission = new MaterialSurfaceTextured(texture);
	}



	inline float Material::shininess() const
	{
		return pShininess;
	}


	inline void Material::shininess(float shininess)
	{
		pShininess = shininess;
	}


	inline float Material::transparency() const
	{
		return pTransparency;
	}


	inline void Material::transparency(float transparency)
	{
		pTransparency = transparency;
	}


	inline float Material::refractionIndex() const
	{
		return pRefractionIndex;
	}


	inline void Material::refractionIndex(float newIndex)
	{
		pRefractionIndex = newIndex;
	}



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_MATERIAL_HXX__
