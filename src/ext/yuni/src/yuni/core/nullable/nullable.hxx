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

	template<class T, class Alloc>
	inline Nullable<T,Alloc>::~Nullable()
	{}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>::Nullable()
	{}


	template<class T, class Alloc>
	template<class U>
	inline Nullable<T,Alloc>::Nullable(const U& rhs) :
		pHolder(rhs)
	{}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>::Nullable(typename Nullable<T,Alloc>::const_pointer rhs)
	{
		if (rhs)
			pHolder.assign(*rhs);
	}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>::Nullable(const Nullable<T,Alloc>& rhs)
		:pHolder(rhs.pHolder)
	{}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>::Nullable(Static::MoveConstructor<Nullable<T,Alloc> > rhs)
		:pHolder(rhs.pHolder)
	{}


	template<class T, class Alloc>
	template<class Alloc1>
	inline Nullable<T,Alloc>::Nullable(const Nullable<T,Alloc1>& rhs)
		:pHolder(rhs.pHolder)
	{}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::iterator Nullable<T,Alloc>::begin()
	{
		return iterator(pHolder.empty() ? NULL : this);
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_iterator Nullable<T,Alloc>::begin() const
	{
		return const_iterator(pHolder.empty() ? NULL : this);
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::iterator Nullable<T,Alloc>::end()
	{
		return iterator();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_iterator Nullable<T,Alloc>::end() const
	{
		return const_iterator();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reverse_iterator Nullable<T,Alloc>::rbegin()
	{
		return reverse_iterator(this);
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_reverse_iterator Nullable<T,Alloc>::rbegin() const
	{
		return const_reverse_iterator(this);
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reverse_iterator Nullable<T,Alloc>::rend()
	{
		return reverse_iterator();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_reverse_iterator Nullable<T,Alloc>::rend() const
	{
		return const_reverse_iterator();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::size_type Nullable<T,Alloc>::size() const
	{
		return pHolder.empty() ? 0 : 1;
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::size_type Nullable<T,Alloc>::capacity()
	{
		return 1;
	}

	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::size_type Nullable<T,Alloc>::max_size()
	{
		return 1;
	}


	template<class T, class Alloc>
	inline void Nullable<T,Alloc>::reserve(typename Nullable<T,Alloc>::size_type)
	{
		/* Do nothing */
	}


	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::empty() const
	{
		return pHolder.empty();
	}


	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::null() const
	{
		return pHolder.empty();
	}



	template<class T, class Alloc>
	template<class Alloc1>
	inline void Nullable<T,Alloc>::swap(Nullable<T,Alloc1>& rhs)
	{
		pHolder.swap(rhs.pHolder);
	}


	template<class T, class Alloc>
	inline void Nullable<T,Alloc>::clear()
	{
		pHolder.clear();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reference Nullable<T,Alloc>::operator * ()
	{
		return pHolder.reference();
	}

	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reference Nullable<T,Alloc>::operator -> ()
	{
		return pHolder.reference();
	}


	template<class T, class Alloc>
	inline void Nullable<T,Alloc>::push_back(const_reference rhs)
	{
		pHolder.assign(rhs);
	}


	template<class T, class Alloc>
	inline void Nullable<T,Alloc>::pop_back()
	{
		pHolder.clear();
	}



	template<class T, class Alloc>
	void Nullable<T,Alloc>::resize(const size_type n, const T& t)
	{
		if (!n)
			pHolder.clear();
		else
		{
			if (pHolder.empty())
				pHolder.assign(t);
		}
	}


	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator ! () const
	{
		return pHolder.empty();
	}


	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator == (const Nullable& rhs) const
	{
		return pHolder.empty()
			? rhs.empty()
			: (rhs.pHolder.data() == pHolder.data());
	}

	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator != (const Nullable& rhs) const
	{
		return pHolder.empty()
			? !rhs.empty()
			: (rhs.pHolder.data() != pHolder.data());
	}



	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator != (const NullPtr&) const
	{
		return !pHolder.empty();
	}

	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator == (const NullPtr&) const
	{
		return pHolder.empty();
	}



	template<class T, class Alloc>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (const Nullable<T,Alloc>& rhs)
	{
		pHolder.assign(rhs);
		return *this;
	}


	template<class T, class Alloc>
	template<class Alloc1>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (const Nullable<T,Alloc1>& rhs)
	{
		pHolder.assign(rhs);
		return *this;
	}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (const NullPtr&)
	{
		pHolder.clear();
		return *this;
	}

	template<class T, class Alloc>
	template<class U>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (const U& rhs)
	{
		pHolder.assign(rhs);
		return *this;
	}


	template<class T, class Alloc>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (typename Nullable<T,Alloc>::const_pointer rhs)
	{
		if (rhs)
			pHolder.assign(*rhs);
		else
			pHolder.clear();
		return *this;
	}




	template<class T, class Alloc>
	inline Nullable<T,Alloc>& Nullable<T,Alloc>::operator = (void* const rhs)
	{
		if (!rhs)
			pHolder.clear();
		else
			throw "Can not assign a nullable with an arbitrary pointer (void*)";
		return *this;
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::Type Nullable<T,Alloc>::value() const
	{
		return pHolder.empty() ? Type() : pHolder.data();
	}

	template<class T, class Alloc>
	template<class U>
	inline typename Nullable<T,Alloc>::Type Nullable<T,Alloc>::value(const U& nullValue) const
	{
		return pHolder.empty() ? nullValue : pHolder.data();
	}




	template<class T, class Alloc>
	template<class S, class U>
	inline void Nullable<T,Alloc>::print(S& out, const U& nullValue) const
	{
		if (pHolder.empty())
			out << nullValue;
		else
			out << pHolder.data();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reference Nullable<T,Alloc>::operator[] (size_type n)
	{
		return pHolder.reference();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_reference Nullable<T,Alloc>::operator[] (size_type n) const
	{
		return pHolder.reference();
	}


	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reference Nullable<T,Alloc>::front()
	{
		return pHolder.reference();
	}

	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::reference Nullable<T,Alloc>::back()
	{
		return pHolder.reference();
	}

	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_reference Nullable<T,Alloc>::front() const
	{
		return pHolder.reference();
	}

	template<class T, class Alloc>
	inline typename Nullable<T,Alloc>::const_reference Nullable<T,Alloc>::back() const
	{
		return pHolder.reference();
	}


	template<class T, class Alloc>
	inline bool Nullable<T,Alloc>::operator < (const Nullable& rhs) const
	{
		return false;
	}





} // namespace Yuni

