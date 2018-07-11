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
#ifndef __YUNI_GFX3D_VERTEX_HXX__
# define __YUNI_GFX3D_VERTEX_HXX__

namespace Yuni
{
namespace Gfx3D
{


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline Vertex<NorP, ColP, TexP, UserP>::Vertex():
		Yuni::Point3D<float>(0.0f, 0.0f, 0.0f)
	{}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	template<class U, class V, class W>
	inline Vertex<NorP, ColP, TexP, UserP>::Vertex(U x, V y, W z):
		Yuni::Point3D<float>(x, y, z)
	{}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline Vertex<NorP, ColP, TexP, UserP>::Vertex(const Vertex& v):
		Yuni::Point3D<float>(v),
		NormalPolicy(v.normal()),
		ColorPolicy(v.color()),
		TexCoordPolicy(v.texCoord()),
		UserDataPolicy(v.user()),
		PaddingT()
	{}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline Vertex<NorP, ColP, TexP, UserP>& Vertex<NorP, ColP, TexP, UserP>::operator = (const Vertex& v)
	{
		// Copy each policy separately. Copying `None` policies will do nothing.
		(*static_cast<Yuni::Point3D<float>*>(this)) = static_cast<const Yuni::Point3D<float>&>(v);
		(*static_cast<NormalPolicy*>(this)) = static_cast<const NormalPolicy&>(v);
		(*static_cast<ColorPolicy*>(this)) = static_cast<const ColorPolicy&>(v);
		(*static_cast<TexCoordPolicy*>(this)) = static_cast<const TexCoordPolicy&>(v);
		(*static_cast<UserDataPolicy*>(this)) = static_cast<const UserDataPolicy&>(v);
		return *this;
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline bool Vertex<NorP, ColP, TexP, UserP>::operator < (const Vertex& v) const
	{
		if (x < v.x)
			return true;
		if (v.x < x)
			return false;
		if (y < v.y)
			return true;
		if (v.y < y)
			return false;
		if (z < v.z)
			return true;
		if (v.z < z)
			return false;
		int cmp = normal().compare(v.normal());
		if (cmp)
			return cmp < 0;
		cmp = color().compare(v.color());
		if (cmp)
			return cmp < 0;
		cmp = texCoord().compare(v.texCoord()) < 0;
		if (cmp)
			return cmp < 0;
		return user().compare(v.user()) < 0;
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline Yuni::Point3D<float>& Vertex<NorP, ColP, TexP, UserP>::position()
	{
		return *static_cast<Yuni::Point3D<float>*>(this);
	}

	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline const Yuni::Point3D<float>& Vertex<NorP, ColP, TexP, UserP>::position() const
	{
		return *static_cast<Yuni::Point3D<float>*>(this);
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline typename Vertex<NorP, ColP, TexP, UserP>::NormalPolicy& Vertex<NorP, ColP, TexP, UserP>::normal()
	{
		return *static_cast<NormalPolicy*>(this);
	}

	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline const typename Vertex<NorP, ColP, TexP, UserP>::NormalPolicy& Vertex<NorP, ColP, TexP, UserP>::normal() const
	{
		return *static_cast<const NormalPolicy*>(this);
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline typename Vertex<NorP, ColP, TexP, UserP>::ColorPolicy& Vertex<NorP, ColP, TexP, UserP>::color()
	{
		return *static_cast<ColorPolicy*>(this);
	}

	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline const typename Vertex<NorP, ColP, TexP, UserP>::ColorPolicy& Vertex<NorP, ColP, TexP, UserP>::color() const
	{
		return *static_cast<const ColorPolicy*>(this);
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline typename Vertex<NorP, ColP, TexP, UserP>::TexCoordPolicy& Vertex<NorP, ColP, TexP, UserP>::texCoord()
	{
		return *static_cast<TexCoordPolicy*>(this);
	}

	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline const typename Vertex<NorP, ColP, TexP, UserP>::TexCoordPolicy& Vertex<NorP, ColP, TexP, UserP>::texCoord() const
	{
		return *static_cast<const TexCoordPolicy*>(this);
	}


	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline typename Vertex<NorP, ColP, TexP, UserP>::UserDataPolicy& Vertex<NorP, ColP, TexP, UserP>::user()
	{
		return *static_cast<UserDataPolicy*>(this);
	}

	template<
		template<class> class NorP,
		template<class> class ColP,
		template<class> class TexP,
		class UserP
	>
	inline const typename Vertex<NorP, ColP, TexP, UserP>::UserDataPolicy& Vertex<NorP, ColP, TexP, UserP>::user() const
	{
		return *static_cast<const UserDataPolicy*>(this);
	}



} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_VERTEX_HXX__
