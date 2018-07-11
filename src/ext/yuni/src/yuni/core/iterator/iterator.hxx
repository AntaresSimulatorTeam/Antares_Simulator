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
#include "iterator.h"



namespace Yuni
{

	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>::IIterator() :
		AncestorType()
	{}


	template<class ModelT, bool ConstT>
	template<class M, bool C>
	inline IIterator<ModelT,ConstT>::IIterator(const IIterator<M,C>& rhs) :
		AncestorType(static_cast<const typename IIterator<M,C>::AncestorType&>(rhs))
	{}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>::IIterator(const IIterator<ModelT,ConstT>& rhs) :
		AncestorType(static_cast<const AncestorType&>(rhs))
	{}


	template<class ModelT, bool ConstT>
	template<class U1>
	inline IIterator<ModelT,ConstT>::IIterator(const U1& u1) :
		AncestorType(u1)
	{}


	template<class ModelT, bool ConstT>
	template<class U1, class U2>
	inline IIterator<ModelT,ConstT>::IIterator(const U1& u1, const U2& u2) :
		AncestorType(u1, u2)
	{}


	template<class ModelT, bool ConstT>
	template<class U1, class U2, class U3>
	inline IIterator<ModelT,ConstT>::IIterator(const U1& u1, const U2& u2, const U3& u3) :
		AncestorType(u1, u2, u3)
	{}


	template<class ModelT, bool ConstT>
	inline void IIterator<ModelT,ConstT>::forward()
	{
		AncestorType::forward();
	}


	template<class ModelT, bool ConstT>
	inline void IIterator<ModelT,ConstT>::forward(typename IIterator<ModelT,ConstT>::difference_type n)
	{
		AncestorType::forward(n);
	}


	template<class ModelT, bool ConstT>
	inline void IIterator<ModelT,ConstT>::backward()
	{
		AncestorType::backward();
	}


	template<class ModelT, bool ConstT>
	inline void IIterator<ModelT,ConstT>::backward(typename IIterator<ModelT,ConstT>::difference_type n)
	{
		AncestorType::backward(n);
	}


	template<class ModelT, bool ConstT>
	inline void IIterator<ModelT,ConstT>::advance(typename IIterator<ModelT,ConstT>::difference_type n)
	{
		if (n > 0)
		{
			AncestorType::forward(n);
		}
		else
		{
			if (n != 0)
				AncestorType::backward(-n);
		}
	}


	template<class ModelT, bool ConstT>
	template<class M, bool C>
	inline typename IIterator<ModelT,ConstT>::difference_type
	IIterator<ModelT,ConstT>::distance(const IIterator<M,C>& rhs) const
	{
		return AncestorType::distance(static_cast<const typename IIterator<M,C>::AncestorType&>(rhs));
	}


	template<class ModelT, bool ConstT>
	inline typename IIterator<ModelT,ConstT>::reference
	IIterator<ModelT,ConstT>::operator * ()
	{
		return AncestorType::operator * ();
	}


	template<class ModelT, bool ConstT>
	inline typename IIterator<ModelT,ConstT>::const_reference
	IIterator<ModelT,ConstT>::operator * () const
	{
		return AncestorType::operator * ();
	}


	template<class ModelT, bool ConstT>
	inline typename IIterator<ModelT,ConstT>::pointer
	IIterator<ModelT,ConstT>::operator -> ()
	{
		return AncestorType::operator -> ();
	}


	template<class ModelT, bool ConstT>
	inline typename IIterator<ModelT,ConstT>::const_pointer
	IIterator<ModelT,ConstT>::operator -> () const
	{
		return AncestorType::operator -> ();
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>&  IIterator<ModelT,ConstT>::operator ++ ()
	{
		AncestorType::forward();
		return *this;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>&  IIterator<ModelT,ConstT>::operator -- ()
	{
		AncestorType::backward();
		return *this;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>  IIterator<ModelT,ConstT>::operator ++ (int)
	{
		IteratorType copy(*this);
		AncestorType::forward();
		return copy;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>  IIterator<ModelT,ConstT>::operator -- (int)
	{
		IteratorType copy(*this);
		AncestorType::backward();
		return copy;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>&  IIterator<ModelT,ConstT>::operator += (typename IIterator<ModelT,ConstT>::difference_type n)
	{
		AncestorType::forward(n);
		return *this;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>&  IIterator<ModelT,ConstT>::operator -= (typename IIterator<ModelT,ConstT>::difference_type n)
	{
		AncestorType::backward(n);
		return *this;
	}


	template<class ModelT, bool ConstT>
	template<class M, bool C>
	inline bool  IIterator<ModelT,ConstT>::operator == (const IIterator<M,C>& rhs) const
	{
		return AncestorType::equals(static_cast<const typename IIterator<M,C>::AncestorType&>(rhs));
	}


	template<class ModelT, bool ConstT>
	template<class M, bool C>
	inline bool  IIterator<ModelT,ConstT>::operator != (const IIterator<M,C>& rhs) const
	{
		return not AncestorType::equals(static_cast<const typename IIterator<M,C>::AncestorType&>(rhs));
	}


	template<class ModelT, bool ConstT>
	template<class M, bool C>
	inline IIterator<ModelT,ConstT>& IIterator<ModelT,ConstT>::operator = (const IIterator<M,C>& rhs)
	{
		AncestorType::reset(static_cast<const typename IIterator<M,C>::AncestorType&>(rhs));
		return *this;
	}


	template<class ModelT, bool ConstT>
	inline IIterator<ModelT,ConstT>& IIterator<ModelT,ConstT>::operator = (const IIterator<ModelT,ConstT>& rhs)
	{
		AncestorType::reset(static_cast<const AncestorType&>(rhs));
		return *this;
	}




} // namespace Yuni
