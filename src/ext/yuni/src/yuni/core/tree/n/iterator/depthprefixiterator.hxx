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
#ifndef __YUNI_CORE_TREE_N_DEPTH_PREFIX_ITERATOR_HXX__
# define __YUNI_CORE_TREE_N_DEPTH_PREFIX_ITERATOR_HXX__

namespace Yuni
{
namespace Private
{
namespace Core
{
namespace Tree
{


	template<class NodeT>
	inline DepthPrefixIterator<NodeT>::DepthPrefixIterator():
		pNode(nullptr)
	{}


	template<class NodeT>
	inline DepthPrefixIterator<NodeT>::DepthPrefixIterator(const Type& it):
		pNode(it.pNode)
	{}


	template<class NodeT>
	template<class N>
	inline DepthPrefixIterator<NodeT>::DepthPrefixIterator(const DepthPrefixIterator<N>& p):
		pNode(p.pNode)
	{}


	template<class NodeT>
	inline DepthPrefixIterator<NodeT>::DepthPrefixIterator(const NodePtr& p):
		pNode(p)
	{}


	template<class NodeT>
	inline typename DepthPrefixIterator<NodeT>::reference
	DepthPrefixIterator<NodeT>::operator * ()
	{
		return *pNode;
	}


	template<class NodeT>
	inline typename DepthPrefixIterator<NodeT>::const_reference
	DepthPrefixIterator<NodeT>::operator * () const
	{
		return *pNode;
	}


	template<class NodeT>
	inline typename DepthPrefixIterator<NodeT>::pointer
	DepthPrefixIterator<NodeT>::operator -> ()
	{
		return pNode.pointer();
	}


	template<class NodeT>
	inline typename DepthPrefixIterator<NodeT>::const_pointer
	DepthPrefixIterator<NodeT>::operator -> () const
	{
		return pNode.pointer();
	}


	template<class NodeT>
	template<class N>
	inline bool DepthPrefixIterator<NodeT>::equals(const DepthPrefixIterator<N>& rhs) const
	{
		return pNode == rhs.pNode;
	}


	template<class NodeT>
	template<class N>
	inline void DepthPrefixIterator<NodeT>::reset(const DepthPrefixIterator<N>& rhs)
	{
		pNode = rhs.pNode;
	}


	template<class NodeT>
	void DepthPrefixIterator<NodeT>::forward()
	{
		// Return the left-most child when possible
		if (!pNode->empty())
		{
			pNode = pNode->firstChild();
			return;
		}
		// If we reached a leaf, do the siblings
		NodePtr next = pNode->nextSibling();
		if (next)
		{
			pNode = next;
			return;
		}
		// Climb back the parents until we find siblings
		while (pNode->parent() and not pNode->parent()->nextSibling())
			pNode = pNode->parent();
		// If there is still no sibling, it means we reached
		// the right-most sibling of the root, which means we have finished.
		if (!pNode->nextSibling())
		{
			pNode = nullptr;
			return;
		}
		// Otherwise, the next sibling we found is the next node to traverse
		pNode = pNode->parent()->nextSibling();
	}


	template<class NodeT>
	void DepthPrefixIterator<NodeT>::forward(difference_type n)
	{
		// FIXME : There is possibility to optimize this a lot
		while (n--)
		{
			forward();
		}
	}


	template<class NodeT>
	void DepthPrefixIterator<NodeT>::backward()
	{
		// If we are on the root, stop
		if (!pNode->parent())
		{
			pNode = nullptr;
			return;
		}
		// If this is the left-most child, go to the parent
		if (!pNode->previousSibling())
		{
			pNode = pNode->parent();
			return;
		}
		// Go to the previous sibling
		pNode = pNode->previousSibling();
		// If the sibling has no child, return it
		while (!pNode->empty())
			pNode = pNode->lastChild();
		// Once we reached a leaf, stop
	}


	template<class NodeT>
	void DepthPrefixIterator<NodeT>::backward(difference_type n)
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

#endif // __YUNI_CORE_TREE_N_DEPTH_PREFIX_ITERATOR_HXX__
