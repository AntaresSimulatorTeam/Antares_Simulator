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
#ifndef __YUNI_GFX3D_VERTEXBUFFERPOLICIES_H__
# define __YUNI_GFX3D_VERTEXBUFFERPOLICIES_H__

# include <yuni/core/point2D.h>
# include <yuni/core/point3D.h>
# include <yuni/core/vector3D.h>
# include <yuni/core/color/rgb.h>
# include <yuni/core/color/rgba.h>
# include <cassert>

# undef None

namespace Yuni
{
namespace Gfx3D
{
namespace Policy
{

namespace Normal
{

	/*!
	** \brief The vertex buffer does not have normal information
	**
	** The fact that the class does not conform to the necessary
	** implicit interfaces will make it fail at compile-time
	*/
	template<class T>
	class None
	{
	public:
		enum
		{
			HasNormal = 0,
			Size = 0
		};

	public:
		//! Assignment operator
		None& operator = (const None&) { return *this; }

		//! Inequality comparison
		int compare(const None&) const { return 0; }

	protected:
		//! Protected empty constructor
		None() {}
		//! Protected copy constructor
		None(const None&) {}
	};


	/*!
	** \brief The vertex buffer has normal information
	*/
	template<class T>
	class With
	{
	public:
		enum
		{
			HasNormal = 1,
			Size = sizeof(Vector3D<float>)
		};

	public:
		Vector3D<float> normal;

	public:
		//! Set the normal from coordinates
		template<class U, class V, class W>
		void operator () (const U x, const V y, const W z)
		{
			normal(x, y, z);
		}

		//! Set the normal from a vector
		template<class U>
		void operator () (const Vector3D<U>& v)
		{
			normal(v);
		}

		//! Assignment operator
		With& operator = (const With& other)
		{
			normal(other.normal);
			return *this;
		}

		//! Inequality comparison
		int compare(const With& other) const
		{
			if (normal.x < other.normal.x)
				return -1;
			if (other.normal.x < normal.x)
				return 1;
			if (normal.y < other.normal.y)
				return -1;
			if (other.normal.y < normal.y)
				return 1;
			if (normal.z < other.normal.z)
				return -1;
			if (other.normal.z < normal.z)
				return 1;
			return 0;
		}

	protected:
		//! Protected empty constructor
		With():
			normal(0.0f, 0.0f, 0.0f)
		{}

		//! Protected copy constructor
		With(const With& other):
			normal(other.normal)
		{}
	};

} // namespace Normal



namespace Color
{

	/*!
	** \brief The vertex buffer does not have color information
	**
	** The fact that the class does not conform to the necessary
	** implicit interfaces will make it fail at compile-time
	*/
	template<class T>
	class None
	{
	public:
		enum
		{
			HasColor = 0,
			Size = 0,
			ElementCount = 0
		};

	public:
		//! Assignment operator
		None& operator = (const None&) { return *this; }

		//! Inequality comparison
		int compare(const None&) const { return 0; }

	protected:
		//! Protected empty constructor
		None() {}
		//! Protected copy constructor
		None(const None&) {}
	};


	/*!
	** \brief The vertex has RGB color information
	*/
	template<class T>
	class RGB
	{
	public:
		enum
		{
			HasColor = 1,
			Size = sizeof(Yuni::Color::RGBA<float>),
			ElementCount = 3
		};

	public:
		Yuni::Color::RGB<float> color;

	public:
		//! Set the color from RGB values
		template<class U, class V, class W>
		void operator () (const U r, const V g, const W b)
		{
			color.assign(r, g, b);
		}

		//! Set the color from another RGB color policy
		void operator () (const RGB& other)
		{
			color.assign(other.color);
		}

		//! Set the color from another RGB color policy
		RGB& operator = (const RGB& other)
		{
			color.assign(other.color);
			return *this;
		}

		//! Inequality comparison
		int compare(const RGB& other) const
		{
			if (color.red < other.color.red)
				return -1;
			if (other.color.red < color.red)
				return 1;
			if (color.green < other.color.green)
				return -1;
			if (other.color.green < color.green)
				return 1;
			if (color.blue < other.color.blue)
				return -1;
			if (other.color.blue < color.blue)
				return 1;
			return 0;
		}

	protected:
		//! Protected empty constructor
		RGB():
			color(0, 0, 0)
		{}

		//! Protected copy constructor
		RGB(const RGB& other):
			color(other.color)
		{}
	};


	/*!
	** \brief The vertex has RGBA color information
	*/
	template<class T>
	class RGBA
	{
	public:
		enum
		{
			HasColor = 1,
			Size = sizeof(Yuni::Color::RGBA<float>),
			ElementCount = 4
		};

	public:
		Yuni::Color::RGBA<float> color;

	public:
		//! Set the color from RGBA values
		template<class U, class V, class W, class X>
		void operator () (const U r, const V g, const W b, const X a)
		{
			color.assign(r, g, b, a);
		}

		//! Set the color from another color
		void operator () (const RGBA& other)
		{
			color.assign(other.color);
		}

		//! Set the color from another color
		RGBA& operator = (const RGBA& other)
		{
			color.assign(other.color);
			return *this;
		}

		//! Inequality comparison
		int compare(const RGBA& other) const
		{
			if (color.red < other.color.red)
				return -1;
			if (other.color.red < color.red)
				return 1;
			if (color.green < other.color.green)
				return -1;
			if (other.color.green < color.green)
				return 1;
			if (color.blue < other.color.blue)
				return -1;
			if (other.color.blue < color.blue)
				return 1;
			if (color.alpha < other.color.alpha)
				return -1;
			if (other.color.alpha < color.alpha)
				return 1;
			return 0;
		}

	protected:
		//! Protected constructor
		RGBA():
			color(0.0f, 0.0f, 0.0f, 0.0f)
			{}

		//! Protected copy constructor
		RGBA(const RGBA& other):
			color(other.color)
		{}
	};

} // namespace Color



namespace TexCoord
{

	/*!
	** \brief The vertex buffer does not have texture coordinate information
	**
	** The fact that the class does not conform to the necessary
	** implicit interfaces will make it fail at compile-time
	*/
	template<class T>
	class None
	{
	public:
		enum
		{
			HasTexture = 0,
			Size = 0,
			ElementCount = 0
		};

	public:
		//! Assignment operator
		None& operator = (const None&) { return *this; }

		//! Inequality comparison
		int compare(const None&) const { return 0; }

	protected:
		//! Protected empty constructor
		None() {}
		//! Protected copy constructor
		None(const None&) {}
	};


	/*!
	** \brief The vertex buffer has a single pair of coordinates from a single texture
	*/
	template<class T>
	class UV
	{
	public:
		enum
		{
			HasTexture = 1,
			Size = 2 * sizeof(float),
			ElementCount = 2
		};

	public:
		float u;
		float v;

	public:
		//! Assignment operator
		UV& operator = (const UV& other)
		{
			u = other.u;
			v = other.v;
			return *this;
		}

		//! Function call operator
		template<class U, class V>
		void operator () (const U newU, const V newV)
		{
			u = newU;
			v = newV;
		}

		//! Function call operator
		template<class U>
		void operator () (const Point2D<U>& coords)
		{
			u = coords.x;
			v = coords.y;
		}

		//! Inequality comparison
		int compare(const UV& other) const
		{
			if (u < other.u)
				return -1;
			if (other.u < u)
				return 1;
			if (v < other.v)
				return -1;
			if (other.v < v)
				return 1;
			return 0;
		}

	protected:
		//! Protected constructor
		UV():
			u(0.0f),
			v(0.0f)
		{}

		//! Copy constructor
		UV(const UV& other):
			u(other.u),
			v(other.v)
		{}

	}; // class UV


	/*!
	** \brief The vertex buffer has different texture info on three sides
	*/
	template<class T>
	class ST3
	{
	public:
		enum
		{
			HasTexture = 1,
			Size = 6 * sizeof(float),
			ElementCount = 6
		};

	public:
		float s0;
		float t0;
		float s1;
		float t1;
		float s2;
		float t2;

	public:
		//! Assignment operator
		ST3& operator = (const ST3& other)
		{
			s0 = other.s0;
			t0 = other.t0;
			s1 = other.s1;
			t1 = other.t1;
			s2 = other.s2;
			t2 = other.t2;
			return *this;
		}

		//! Inequality comparison
		int compare(const ST3& other) const
		{
			if (s0 < other.s0)
				return -1;
			if (other.s0 < s0)
				return 1;
			if (t0 < other.t0)
				return -1;
			if (other.t0 < t0)
				return 1;
			if (s1 < other.s1)
				return -1;
			if (other.s1 < s1)
				return 1;
			if (t1 < other.t1)
				return -1;
			if (other.t1 < t1)
				return 1;
			if (s2 < other.s2)
				return -1;
			if (other.s2 < s2)
				return 1;
			if (t2 < other.t2)
				return -1;
			if (other.t2 < t2)
				return 1;
			return 0;
		}

	protected:
		//! Protected constructor
		ST3():
			s0(0.0f),
			t0(0.0f),
			s1(0.0f),
			t1(0.0f),
			s2(0.0f),
			t2(0.0f)
		{}

		//! Copy constructor
		ST3(const ST3& other):
			s0(other.s0),
			t0(other.t0),
			s1(other.s1),
			t1(other.t1),
			s2(other.s2),
			t2(other.t2)
		{}

	}; // class ST3


} // namespace TexCoord


namespace User
{

	/*!
	** \brief The vertex buffer does not have any user data
	*/
	class None
	{
	public:
		enum
		{
			HasUser = 0,
			Size = 0,
			ElementCount = 0
		};

	public:
		//! Assignment operator
		None& operator = (const None&) { return *this; }

		//! Inequality comparison
		int compare(const None&) const { return 0; }

	protected:
		//! Protected empty constructor
		None() {}
		//! Protected copy constructor
		None(const None&) {}

	}; // class None


	/*!
	** \brief The vertex buffer contains a value of type UserT as user data
	**
	** \warning UserT must provide empty and copy constructors, as well as operators < and =
	*/
	template<class UserT>
	class With
	{
	public:
		enum // anonymous
		{
			HasUser = 1,
			Size = sizeof (UserT),
			ElementCount = 1

		}; // enum anonymous

	public:
		//! Data type
		typedef UserT  Type;

	public:
		//! Assignment operator
		With& operator = (const With& other)
		{
			data = other.data;
			return *this;
		}

		//! Function call operator
		template<class T>
		void operator () (const T newData)
		{
			data = static_cast<UserT>(newData);
		}

		//! Inequality comparison
		int compare(const With& other) const
		{
			if (data < other.data)
				return -1;
			if (other.data < data)
				return 1;
			return 0;
		}

	public:
		UserT data;

	protected:
		//! Protected empty constructor
		With():
			data(UserT())
		{}

		//! Protected copy constructor
		With(const With& other):
			data(other.data)
		{}

	}; // class With


} // namespace User

} // namespace Policy
} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_VERTEXBUFFERPOLICIES_H__
