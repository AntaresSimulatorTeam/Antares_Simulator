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
#pragma once



namespace Yuni
{

	template<typename T, class Alloc> class YUNI_DECL Nullable;


namespace Private
{
namespace NullableImpl
{

	template<class T>
	struct IsNullable final
	{
		enum { Yes = 0, No = 1 };
	};

	template<class U, class Alloc>
	struct IsNullable< Nullable<U, Alloc> > final
	{
		enum { Yes = 1, No = 0 };
	};





	// Forward declaration
	template<class T, int S>
	class YUNI_DECL Holder final
	{
	public:
		Holder() :
			pHasData(false)
		{}
		Holder(const Holder& rhs) :
			pData(rhs.pData), pHasData(rhs.pHasData)
		{}
		template<typename U> Holder(const U& rhs) :
			pData(rhs), pHasData(true)
		{}

		void clear() {pHasData = false;}

		bool empty() const {return !pHasData;}

		T& reference()
		{
			if (!pHasData)
			{
				pHasData = true;
				pData = T();
			}
			return pData;
		}

		const T& data() const {return pData;}
		T& data() {return pData;}

		template<class U>
		void assign(const U& v)
		{
			pData = v;
			pHasData = true;
		}

		void assign(const NullPtr&)
		{
			pHasData = false;
		}

		void assign(const Holder& rhs)
		{
			pHasData = rhs.pHasData;
			pData = rhs.pData;
		}

	private:
		//! The data
		T pData;
		//
		bool pHasData;

	};


	template<class T>
	class YUNI_DECL Holder<T,0> final
	{
	public:
		Holder()
		{}
		Holder(const Holder& rhs) :
			pData(rhs.pData)
		{}
		template<typename U> Holder(const U& rhs) :
			pData(new T(rhs))
		{}

		void clear()
		{
			pData = nullptr;
		}

		bool empty() const {return !pData;}

		T& reference()
		{
			if (!pData)
				pData = new T();
			return *pData;
		}

		const T& data() const {return *pData;}
		T& data() {return *pData;}

		template<class U>
		void assign(const U& v)
		{
			if (!pData)
				pData = new T(v);
			else
				*pData = v;
		}

		void assign(const NullPtr&)
		{
			pData = nullptr;
		}


		void assign(const Holder& rhs)
		{
			pData = rhs.pData;
		}

		void swap(Holder& rhs)
		{
			pData.swap(rhs.pData);
		}

	private:
		/*!
		** \brief Pointer to the data
		**
		** The pointer is actually a smartptr to handle the copy constructor efficiently
		** for the class `Holder`.
		*/
		typename T::Ptr pData;

	}; // class Holder





} // namespace NullableImpl
} // namespace Private
} // namespace Yuni

