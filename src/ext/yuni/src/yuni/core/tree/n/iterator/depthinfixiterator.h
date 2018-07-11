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
namespace Private
{
namespace Core
{
namespace Tree
{


	template<class NodeT>
	class DepthInfixIterator
	{
	public:
		typedef DepthInfixIterator<NodeT>  Type;

		typedef typename NodeT::Ptr NodePtr;

		typedef NodeT  value_type;
		typedef int  difference_type;
		typedef value_type&  reference;
		typedef const value_type&  const_reference;
		typedef value_type*  pointer;
		typedef const value_type*  const_pointer;

	public:
		enum
		{
			canGoForward = true,
			canGoBackward = true,
		};

	public:
		//! \name Constructors
		//@{
		DepthInfixIterator();

		DepthInfixIterator(const Type& it);

		template<class N>
		DepthInfixIterator(const DepthInfixIterator<N>& p);

		DepthInfixIterator(const NodePtr& p);
		//@}

		//! \name Static overloads from IIterator
		//@{
		void forward();

		void forward(difference_type n);

		void backward();

		void backward(difference_type n);

		void advance(difference_type n);

		template<class N>
		bool equals(const DepthInfixIterator<N>& rhs) const;

		template<class N>
		void reset(const DepthInfixIterator<N>& rhs);

		template<class N>
		difference_type distance(const DepthInfixIterator<N>& rhs) const;
		//@}


		//! \name Operator overloads
		//@{

		//! The operator `*`
		reference operator * ();

		//! The operator `*`
		const_reference operator * () const;

		//! The operator `->`
		pointer operator -> ();

		//! The operator `->`
		const_pointer operator -> () const;

		//@}

	private:
		//! The actual data
		NodePtr pNode;
	}; // class DepthInfixIterator



} // namespace Tree
} // namespace Core
} // namespace Private
} // namespace Yuni

#include "depthinfixiterator.hxx"
