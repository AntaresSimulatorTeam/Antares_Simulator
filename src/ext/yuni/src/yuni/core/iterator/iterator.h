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
#include "../../yuni.h"
#include "../static/if.h"
#include <iterator> // iterator traits from STL


namespace Yuni
{

	/*!
	** \brief Generic Iterator
	*/
	template<class ModelT, bool ConstT>
	class IIterator : public ModelT
	{
	public:
		//! The model implementation
		typedef ModelT ModelType;
		//! The iterator type
		typedef IIterator<ModelT,ConstT> IteratorType;
		//! Category
		typedef IteratorType iterator_category;

		//! Ancestor
		typedef ModelT AncestorType;

		//! Value type
		typedef typename ModelT::value_type  value_type;
		//! Difference type
		typedef typename ModelT::difference_type difference_type;
		//! Pointer
		typedef typename Static::If<ConstT, const value_type*, value_type*>::Type pointer;
		//! Const pointer
		typedef const value_type* const_pointer;
		//! Reference
		typedef typename Static::If<ConstT, const value_type&, value_type&>::Type reference;
		//! Const reference
		typedef const value_type& const_reference;
		//! Const this if required
		typedef typename Static::If<ConstT, const AncestorType*, AncestorType*>::Type ConstSelfPtr;
		enum
		{
			//! A non-zero value for const qualifier
			isConst = ConstT,
			//! A non-zero value if the iterator can go forward
			canGoForward = ModelT::canGoForward,
			//! A non-zero value if the iterator can go backward
			canGoBackward = ModelT::canGoBackward,
		};

	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		IIterator();

		/*!
		** \brief Copy constructor
		*/
		IIterator(const IteratorType& rhs);

		/*!
		** \brief Copy constructor from any iterator
		*/
		template<class M, bool C> IIterator(const IIterator<M,C>& rhs);

		/*!
		** \brief Assignation from a custom type, defined by the real implementation
		*/
		template<class U1> explicit IIterator(const U1& u1);

		/*!
		** \brief Assignation from two custom types, defined by the real implementation
		*/
		template<class U1, class U2> IIterator(const U1& u1, const U2& u2);

		/*!
		** \brief Assignation from three custom types, defined by the real implementation
		*/
		template<class U1, class U2, class U3> IIterator(const U1& u1, const U2& u2, const U3& u3);
		//@}


		//! \name Common operations
		//@{
		/*!
		** \brief Increment the iterator by the distance 1
		*/
		void forward();

		/*!
		** \brief Increment the iterator by the distance n
		*/
		void forward(difference_type n);

		/*!
		** \brief Decrement the iterator by the distance n
		*/
		void backward();

		/*!
		** \brief Decrement the iterator by the distance n
		*/
		void backward(difference_type n);

		/*!
		** \brief Increment/Decrement the iterator by the distance n
		**
		** If n > 0 it is equivalent to executing ++i n times, and if n < 0 it is
		** equivalent to executing --i n times. If n == 0, the call has no effect.
		*/
		void advance(difference_type n);

		/*!
		** \brief Get the distance between this iterator and another one
		*/
		template<class M, bool C>
		difference_type distance(const IIterator<M,C>& rhs) const;
		//@}


		//! \name Operators
		//@{
		//! Dereferencing
		reference operator * ();
		//! Dereferencing (const)
		const_reference operator * () const;

		//! Dereferencing
		pointer operator -> ();
		//! Dereferencing (const)
		const_pointer operator -> () const;

		//! Increment the operator by the distance 1
		IteratorType& operator ++ ();
		//! Decrement the iterator by the distance 1
		IteratorType& operator -- ();

		//! (Post) Increment the iterator by the distance 1
		IteratorType operator ++ (int);
		//! (Post) Decrement the iterator by the distance 1
		IteratorType operator -- (int);

		//! Increment the operator by the distance n
		IteratorType& operator += (difference_type n);
		//! Decrement the operator by the distance n
		IteratorType& operator -= (difference_type n);

		//! Equality comparison
		template<class M, bool C> bool operator == (const IIterator<M,C>& rhs) const;
		//! Non-equality comparison
		template<class M, bool C> bool operator != (const IIterator<M,C>& rhs) const;

		//! Assignment
		IteratorType& operator = (const IteratorType& rhs);
		//! Assignment
		template<class M, bool C> IteratorType& operator = (const IIterator<M,C>& rhs);
		//@}


	protected:
		// Friend
		template<class M, bool C> friend class IIterator;

	}; // class IIterator





} // namespace Yuni



# include "iterator.hxx"




template<class ModelT, bool ConstT>
inline Yuni::IIterator<ModelT,ConstT>
operator + (const Yuni::IIterator<ModelT,ConstT>& it, typename Yuni::IIterator<ModelT,ConstT>::difference_type n)
{
	return (Yuni::IIterator<ModelT,ConstT>(it) += n);
}

template<class ModelT, bool ConstT>
inline Yuni::IIterator<ModelT,ConstT>
operator + (typename Yuni::IIterator<ModelT,ConstT>::difference_type n, const Yuni::IIterator<ModelT,ConstT>& it)
{
	return (Yuni::IIterator<ModelT,ConstT>(it) += n);
}


template<class ModelT, bool ConstT>
inline Yuni::IIterator<ModelT,ConstT>
operator - (const Yuni::IIterator<ModelT,ConstT>& it, typename Yuni::IIterator<ModelT,ConstT>::difference_type n)
{
	return (Yuni::IIterator<ModelT,ConstT>(it) -= n);
}


template<class ModelT, bool ConstT>
inline Yuni::IIterator<ModelT,ConstT>
operator - (typename Yuni::IIterator<ModelT,ConstT>::difference_type n, const Yuni::IIterator<ModelT,ConstT>& it)
{
	return (Yuni::IIterator<ModelT,ConstT>(it) -= n);
}
