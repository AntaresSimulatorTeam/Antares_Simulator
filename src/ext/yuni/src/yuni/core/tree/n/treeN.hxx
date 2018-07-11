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
#include "treeN.h"



namespace Yuni
{
namespace Core
{


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline TreeN<T,TP,ChckP,ConvP>::TreeN()
		: pParent(nullptr)
		, pChildrenCount(0)
		, pRefCount(0)
	{}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline TreeN<T,TP,ChckP,ConvP>::~TreeN()
	{
		// The node should be detached as soon as possible to avoid
		// any operation on it
		detachFromParent();
		// Remove all children
		clear();
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::empty() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return (0 == pChildrenCount);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::leaf() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return (0 == pChildrenCount);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline typename TreeN<T,TP,ChckP,ConvP>::SizeType TreeN<T,TP,ChckP,ConvP>::count() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return pChildrenCount;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline typename TreeN<T,TP,ChckP,ConvP>::SizeType TreeN<T,TP,ChckP,ConvP>::size() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return pChildrenCount;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void TreeN<T,TP,ChckP,ConvP>::detachFromParent()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		if (pParent)
			detachFromParentWL();
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void TreeN<T,TP,ChckP,ConvP>::detachFromParentWL()
	{
		assert(pParent != NULL);
		// Remove the reference from the parent
		pParent->internalRemoveChild(*(static_cast<Node*>(this)));
		// Removing our references to the parent
		pParent          = nullptr;
		pPreviousSibling = nullptr;
		pNextSibling     = nullptr;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void TreeN<T,TP,ChckP,ConvP>::parent(Ptr newParent)
	{
		detachFromParent();
		newParent->push_back(static_cast<Node*>(this));
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::internalRemoveChild(Node& node)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return internalRemoveChildWL(node);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	bool TreeN<T,TP,ChckP,ConvP>::internalRemoveChildWL(Node& node)
	{
		if (!node.pPreviousSibling)
		{
			// It was the first child actually
			pFirstChild = node.pNextSibling;
		}
		else
		{
			// Updating the next sibling of the previous one
			node.pPreviousSibling->pNextSibling = node.pNextSibling;
		}
		if (!node.pNextSibling)
		{
			// It was the last child actually
			pLastChild = node.pPreviousSibling;
		}
		else
		{
			// Updating the previous sibling of the next one
			node.pNextSibling->pPreviousSibling = node.pPreviousSibling;
		}
		// We have lost a child anyway :'(
		--pChildrenCount;
		return true;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::remove(Ptr& node)
	{
		// only the real parent can remove a node
		return (Ptr::WeakPointer(node->parent()) == this)
			? internalRemoveChild(*node)
			: false;
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	typename TreeN<T,TP,ChckP,ConvP>::Ptr
	TreeN<T,TP,ChckP,ConvP>::findFromIndexWL(const typename TreeN<T,TP,ChckP,ConvP>::SizeType index)
	{
		if (index < pChildrenCount)
		{
			// A little trick to spee-up the reseach when dealing with huge amount
			// of children
			if (index < pChildrenCount - index)
			{
				Ptr* it = &pFirstChild;
				SizeType i = 0;
				while (index != i)
				{
					it = &((*it)->pNextSibling);
					++i;
				}
				return *it;
			}
			else
			{
				Ptr* it = &pLastChild;
				SizeType i = pChildrenCount - 1;
				while (index != i)
				{
					it = &((*it)->pPreviousSibling);
					--i;
				}
				return *it;
			}
		}
		return Ptr();
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline typename TreeN<T,TP,ChckP,ConvP>::Ptr
	TreeN<T,TP,ChckP,ConvP>::find(const typename TreeN<T,TP,ChckP,ConvP>::SizeType index)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return findFromIndexWL(index);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline typename TreeN<T,TP,ChckP,ConvP>::Ptr
	TreeN<T,TP,ChckP,ConvP>::find(const typename TreeN<T,TP,ChckP,ConvP>::SignedSizeType index)
	{
		if (index >= 0)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			return findFromIndexWL((SignedSizeType)index);
		}
		return NULL;
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool
	TreeN<T,TP,ChckP,ConvP>::remove(const typename TreeN<T,TP,ChckP,ConvP>::SizeType index)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		Ptr* p = findFromIndexWL(index);
		return (p) ? internalRemoveChildWL(p) : false;
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool
	TreeN<T,TP,ChckP,ConvP>::remove(const typename TreeN<T,TP,ChckP,ConvP>::SignedSizeType index)
	{
		if (index >= 0)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			Ptr* p = findFromIndexWL((typename Node::SizeType)index);
			return (p) ? internalRemoveChildWL(p) : false;
		}
		return false;
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void
	TreeN<T,TP,ChckP,ConvP>::internalDetachFromParentWithoutNotify()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		// Removing all our children before
		clearWL();
		// Removing our references to the parent
		pParent          = nullptr;
		pPreviousSibling = nullptr;
		pNextSibling     = nullptr;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void TreeN<T,TP,ChckP,ConvP>::clear()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		if (pChildrenCount)
			clearWL();
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void TreeN<T,TP,ChckP,ConvP>::clearWL()
	{
		{
			std::stack<Ptr> queue;

			// Getting a copy of the list of all children
			const iterator endIt = end();
			for (iterator i = begin(); i != endIt; ++i)
				queue.push(&*i);

			// Now we have a copy of the list, we can safely remove the links
			// with our children

			while (!queue.empty())
			{
				queue.top()->internalDetachFromParentWithoutNotify();
				queue.pop();
			}
		}

		pLastChild  = nullptr;
		pFirstChild = nullptr;
		pChildrenCount = 0;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::append(typename TreeN<T,TP,ChckP,ConvP>::Ptr& node)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushBackWL(node);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::append(T* node)
	{
		Ptr tmp(node);
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushBackWL(tmp);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::push_back(T* node)
	{
		Ptr tmp(node);
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushBackWL(tmp);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::push_front(T* node)
	{
		Ptr tmp(node);
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushFrontWL(tmp);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::push_back(Ptr& node)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushBackWL(node);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void
	TreeN<T,TP,ChckP,ConvP>::pushBackWL(Ptr& node)
	{
		if (!pChildrenCount)
		{
			pFirstChild = node;
			pLastChild = pFirstChild;
			pFirstChild->pPreviousSibling = nullptr;
			pFirstChild->pNextSibling = nullptr;
			pChildrenCount = 1;
		}
		else
		{
			pLastChild->pNextSibling = node;
			node->pPreviousSibling = pLastChild;
			node->pNextSibling = nullptr;
			pLastChild = node;
			++pChildrenCount;
		}
		node->pParent = static_cast<Node*>(this);
	}




	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::push_front(Ptr& node)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		pushFrontWL(node);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void
	TreeN<T,TP,ChckP,ConvP>::pushFrontWL(Ptr& node)
	{
		if (!pChildrenCount)
		{
			pFirstChild = node;
			pLastChild = pFirstChild;
			pFirstChild->pPreviousSibling = nullptr;
			pFirstChild->pNextSibling = nullptr;
			pChildrenCount = 1;
		}
		else
		{
			node->pNextSibling = pFirstChild;
			node->pPreviousSibling = nullptr;
			pFirstChild->pPreviousSibling = node;
			pFirstChild = node;
			++pChildrenCount;
		}
		node->pParent = static_cast<Node*>(this);
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::printBeginWL(std::ostream& out, uint) const
	{
		out << "<node>";
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::printEndWL(std::ostream& out, uint) const
	{
		out << "</node>";
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	std::ostream&
	TreeN<T,TP,ChckP,ConvP>::print(std::ostream& out, bool recursive, uint level)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		for (uint i = 0; i < level; ++i)
			out << "    ";
		printBeginWL(out, level);
		out << "\n";

		if (recursive and pChildrenCount)
		{
			const iterator end;
			for (iterator i = begin(); i != end; ++i)
				i->print(out, true, level + 1);
		}

		for (uint i = 0; i < level; ++i)
			out << "    ";
		printEndWL(out, level);
		out << '\n';
		return out;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::equals(const Ptr& node) const
	{
		return (this == node);
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	typename TreeN<T,TP,ChckP,ConvP>::SizeType
	TreeN<T,TP,ChckP,ConvP>::depth() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return (pParent) ? (1 + pParent->depth()) : 0;
	}




	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	typename TreeN<T,TP,ChckP,ConvP>::SizeType
	TreeN<T,TP,ChckP,ConvP>::treeHeight()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		if (!pChildrenCount)
			return 0;
		SizeType d = 0;

		const iterator end;
		for (iterator i = begin(); i != end; ++i)
		{
			SizeType nd = i->treeHeight();
			if (nd > d)
				d = nd;
		}

		return d + 1;
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void TreeN<T,TP,ChckP,ConvP>::bringToFront()
	{
		// Locking
		typename ThreadingPolicy::MutexLocker locker(*this);

		// We check in a first time if we are not already at the end
		if (pParent and pNextSibling)
		{
			// Locking the parent
			typename ThreadingPolicy::MutexLocker locker(pParent);

			// Ok we have to move
			detachFromParentWL();
			pParent->pushBackWL(this);
		}
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	void
	TreeN<T,TP,ChckP,ConvP>::sendToBack()
	{
		// Locking
		typename ThreadingPolicy::MutexLocker locker(*this);

		// We check in a first time if we are not already at the end
		if (pParent and pPreviousSibling)
		{
			typename ThreadingPolicy::MutexLocker locker(pParent);
			detachFromParentWL();
			pParent->pushFrontWL(this);
		}
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::invalidate()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		invalidateWL();
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool
	TreeN<T,TP,ChckP,ConvP>::isInvalidated()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return isInvalidatedWL();
	}



	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline void
	TreeN<T,TP,ChckP,ConvP>::addRef() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		++pRefCount;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	bool TreeN<T,TP,ChckP,ConvP>::release() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		assert(pRefCount > 0 and "TreeN: invalid reference count");
		if (--pRefCount != 0)
			return false;

		// Early clean-up
		// The method 'release' must be const for good code design. But
		// we have to be properly detached from the parent node.
		TreeNNode& ref = *(const_cast<TreeNNode*>(this));
		if (pParent)
			ref.detachFromParentWL();
		if (pChildrenCount)
			ref.clearWL();
		return true;
	}


	template<class T, template<class> class TP, template <class> class ChckP,
		class ConvP>
	inline bool TreeN<T,TP,ChckP,ConvP>::hasIntrusiveSmartPtr() const
	{
		return true;
	}




} // namespace Core
} // namespace Yuni
