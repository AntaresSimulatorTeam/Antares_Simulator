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
	inline DepthInfixIterator<NodeT>::DepthInfixIterator():
		pNode(nullptr)
	{}


	template<class NodeT>
	inline DepthInfixIterator<NodeT>::DepthInfixIterator(const Type& it):
		pNode(it.pNode)
	{}


	template<class NodeT>
	template<class N>
	inline DepthInfixIterator<NodeT>::DepthInfixIterator(const DepthInfixIterator<N>& p):
		pNode(p.pNode)
	{}


	template<class NodeT>
	inline DepthInfixIterator<NodeT>::DepthInfixIterator(const NodePtr& p):
		pNode(p)
	{}


	template<class NodeT>
	inline typename DepthInfixIterator<NodeT>::reference
	DepthInfixIterator<NodeT>::operator * ()
	{
		return *pNode;
	}


	template<class NodeT>
	inline typename DepthInfixIterator<NodeT>::const_reference
	DepthInfixIterator<NodeT>::operator * () const
	{
		return *pNode;
	}


	template<class NodeT>
	inline typename DepthInfixIterator<NodeT>::pointer
	DepthInfixIterator<NodeT>::operator -> ()
	{
		return pNode.pointer();
	}


	template<class NodeT>
	inline typename DepthInfixIterator<NodeT>::const_pointer
	DepthInfixIterator<NodeT>::operator -> () const
	{
		return pNode.pointer();
	}


	template<class NodeT>
	template<class N>
	inline bool DepthInfixIterator<NodeT>::equals(const DepthInfixIterator<N>& rhs) const
	{
		return pNode == rhs.pNode;
	}


	template<class NodeT>
	template<class N>
	inline void DepthInfixIterator<NodeT>::reset(const DepthInfixIterator<N>& rhs)
	{
		pNode = rhs.pNode;
	}


	template<class NodeT>
	void DepthInfixIterator<NodeT>::forward()
	{
		// TODO
	}


	template<class NodeT>
	void DepthInfixIterator<NodeT>::forward(difference_type n)
	{
		// FIXME : There is possibility to optimize this a lot
		while (n--)
		{
			forward();
		}
	}


	template<class NodeT>
	void DepthInfixIterator<NodeT>::backward()
	{
		// TODO
	}


	template<class NodeT>
	void DepthInfixIterator<NodeT>::backward(difference_type n)
	{
		// FIXME : There is possibility to optimize this a lot
		while (n--)
		{
			backward();
		}
	}



} // namespace Tree
} // namespace Core
} // namespace Private
} // namespace Yuni
