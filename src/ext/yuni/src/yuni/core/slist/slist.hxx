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
#include "slist.h"



namespace Yuni
{
namespace Private
{
namespace LinkedListImpl
{

	template<int ComparePointer>
	struct Compare final
	{
		template<class U, class V>
		static bool Perform(const U& u, const V& v)
		{
			return (&u) == (&v);
		}
	};


	template<>
	struct Compare<0> final
	{
		template<class U, class V>
		static bool Perform(const U& u, const V& v)
		{
			return u == v;
		}
	};


} // namespace LinkedListImpl
} // namespace Private
} // namespace Yuni





namespace Yuni
{


	template<class T, class Alloc>
	inline LinkedList<T,Alloc>::LinkedList() :
		pHead(NULL), pLast(NULL), pCount(0)
	{}


	template<class T, class Alloc>
	inline LinkedList<T,Alloc>::LinkedList(const LinkedList<T, Alloc>& rhs) :
		pHead(NULL), pLast(NULL), pCount(0)
	{
		push_back(rhs);
	}


	# ifdef YUNI_HAS_CPP_MOVE
	template<class T, class Alloc>
	inline LinkedList<T,Alloc>::LinkedList(LinkedList<T, Alloc>&& rhs) :
		pHead(rhs.pHead), pLast(rhs.pLast), pCount(rhs.pCount)
	{
		rhs.pHead  = nullptr;
		rhs.pLast  = nullptr;
		rhs.pCount = 0;
	}
	# endif


	template<class T, class Alloc>
	template<class U, class A>
	inline LinkedList<T,Alloc>::LinkedList(const LinkedList<U,A>& rhs) :
		pHead(NULL), pLast(NULL), pCount(0)
	{
		push_back(rhs);
	}


	template<class T, class Alloc>
	template<class U, class A>
	inline LinkedList<T,Alloc>::LinkedList(const std::list<U,A>& rhs) :
		pHead(NULL), pLast(NULL), pCount(0)
	{
		push_back(rhs);
	}


	template<class T, class Alloc>
	template<class U, class A>
	inline LinkedList<T,Alloc>::LinkedList(const std::vector<U,A>& rhs) :
		pHead(NULL), pLast(NULL), pCount(0)
	{
		push_back(rhs);
	}


	template<class T, class Alloc>
	inline LinkedList<T,Alloc>::~LinkedList()
	{
		// Destroy all items if not empty
		if (pHead)
			doClear();
	}


	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::reference_type LinkedList<T,Alloc>::front()
	{
		assert(pHead and "the list is empty");
		return pHead->data;
	}

	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::const_reference_type LinkedList<T,Alloc>::front() const
	{
		assert(pHead and "the list is empty");
		return pHead->data;
	}

	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::reference_type LinkedList<T,Alloc>::back()
	{
		assert(pHead and "the list is empty");
		return pLast->data;
	}

	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::const_reference_type LinkedList<T,Alloc>::back() const
	{
		assert(pHead and "the list is empty");
		return pLast->data;
	}




	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::iterator LinkedList<T,Alloc>::begin()
	{
		return iterator(pHead);
	}


	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::const_iterator LinkedList<T,Alloc>::begin() const
	{
		return const_iterator(pHead);
	}


	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::iterator LinkedList<T,Alloc>::end()
	{
		return iterator();
	}


	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::const_iterator LinkedList<T,Alloc>::end() const
	{
		return const_iterator();
	}


	template<class T, class Alloc>
	typename LinkedList<T,Alloc>::iterator
	LinkedList<T,Alloc>::find(typename LinkedList<T,Alloc>::const_reference_type value)
	{
		Item* it = pHead;
		while (it)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, it->data))
				return iterator(it);
			it = it->next;
		}
		return iterator();
	}


	template<class T, class Alloc>
	template<class U>
	typename LinkedList<T,Alloc>::iterator
	LinkedList<T,Alloc>::find(const U& value)
	{
		Item* it = pHead;
		while (it)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, it->data))
				return iterator(it);
			it = it->next;
		}
		return iterator();
	}


	template<class T, class Alloc>
	template<class U>
	typename LinkedList<T,Alloc>::const_iterator
	LinkedList<T,Alloc>::find(const U& value) const
	{
		Item* it = pHead;
		while (it)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, it->data))
				return iterator(it);
			it = it->next;
		}
		return iterator();
	}

	template<class T, class Alloc>
	typename LinkedList<T,Alloc>::const_iterator
	LinkedList<T,Alloc>::find(typename LinkedList<T,Alloc>::const_reference_type value) const
	{
		Item* it = pHead;
		while (it)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, it->data))
				return iterator(it);
			it = it->next;
		}
		return iterator();
	}



	template<class T, class Alloc>
	void LinkedList<T,Alloc>::push_back()
	{
		if (pLast)
		{
			pLast->next = new Item();
			pLast = pLast->next;
			++pCount;
		}
		else
		{
			pHead = new Item();
			pLast = pHead;
			pCount = 1;
		}
	}


	template<class T, class Alloc>
	template<class U>
	inline void LinkedList<T,Alloc>::push_back(const U& value)
	{
		if (pLast)
		{
			pLast->next = new Item(value);
			pLast = pLast->next;
			++pCount;
		}
		else
		{
			pHead = new Item(value);
			pLast = pHead;
			pCount = 1;
		}
	}


	template<class T, class Alloc>
	inline void LinkedList<T,Alloc>::push_back(reference_type value)
	{
		if (pLast)
		{
			pLast->next = new Item(value);
			pLast = pLast->next;
			++pCount;
		}
		else
		{
			pHead = new Item(value);
			pLast = pHead;
			pCount = 1;
		}
	}


	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_back(const LinkedList<U,A>& rhs)
	{
		Item* cursor = rhs.pHead;
		while (cursor)
		{
			push_front(cursor->data);
			cursor = cursor->next;
		}
	}


	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_back(const std::list<U,A>& rhs)
	{
		typedef std::list<U,A> L;
		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			push_back(*i);
	}


	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_back(const std::vector<U,A>& rhs)
	{
		typedef std::vector<U,A> L;
		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			push_back(*i);
	}


	template<class T, class Alloc>
	void LinkedList<T,Alloc>::push_front()
	{
		if (pHead)
		{
			pHead = new Item(pHead);
			++pCount;
		}
		else
		{
			pHead = new Item();
			pLast = pHead;
			pCount = 1;
		}
	}


	template<class T, class Alloc>
	template<class U>
	void LinkedList<T,Alloc>::push_front(const U& value)
	{
		if (pHead)
		{
			pHead = new Item(pHead, value);
			++pCount;
		}
		else
		{
			pHead = new Item(value);
			pLast = pHead;
			pCount = 1;
		}
	}

	template<class T, class Alloc>
	void LinkedList<T,Alloc>::push_front(reference_type value)
	{
		if (pHead)
		{
			pHead = new Item(pHead, value);
			++pCount;
		}
		else
		{
			pHead = new Item(value);
			pLast = pHead;
			pCount = 1;
		}
	}



	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_front(const LinkedList<U,A>& rhs)
	{
		Item* cursor = rhs.pHead;
		while (cursor)
		{
			push_front(cursor->data);
			cursor = cursor->next;
		}
	}


	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_front(const std::list<U,A>& rhs)
	{
		typedef std::list<U,A> L;
		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			push_back(*i);
	}


	template<class T, class Alloc>
	template<class U, class A>
	void LinkedList<T,Alloc>::push_front(const std::vector<U,A>& rhs)
	{
		typedef std::vector<U,A> L;
		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			push_back(*i);
	}


	template<class T, class Alloc>
	void LinkedList<T,Alloc>::pop_front()
	{
		if (pHead)
		{
			Item* it = pHead;
			pHead = it->next;
			delete it;
			if (!pHead)
				pLast = NULL;
			--pCount;
		}
	}


	template<class T, class Alloc>
	void LinkedList<T,Alloc>::erase(iterator& i)
	{
		// Make sure the list is not empty
		if (not pHead)
			return;

		// If we are erasing the first item
		if (pHead == i.pCurrent)
		{
			pHead = pHead->next;
			delete i.pCurrent;
			--pCount;
		}
		// Otherwise, search for the item to delete
		else
		{
			for (Item* it = pHead; it->next; ++it)
			{
				// Is it the correct item ?
				if (it->next == i)
				{
					// Remove from the list and delete
					it->next = it->next->next;
					delete it->next;
					--pCount;
					break;
				}
			}
		}
	}


	template<class T, class Alloc>
	template<class U>
	typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::remove(const U& value)
	{
		// !!! DO not forget to push changes into remove(reference_type) as well

		// The current position in the list
		Item* cursor = pHead;
		// The previous item to 'cursor'
		Item* previous = NULL;
		Size removeCount = 0;

		// For each item in the list...
		while (cursor)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, cursor->data))
			{
				// We're keeping a reference to the item to remove
				Item* itemToDelete = cursor;
				// The previous valid item remains the same
				cursor = cursor->next;

				// Removing the item from the list
				if (previous)
				{
					// We have to remove the item in the middle of nowhere
					// We have to update the reference of the previous item
					previous->next = cursor; // the same than itemToDelete->next
					// The reference to the previous item may need an update
					if (pLast == itemToDelete)
						pLast = previous;
					--pCount;
				}
				else
				{
					// The item to remove is actually the first item
					// Two cases :
					// The first one we have more than one item in the list
					if (pLast != pHead)
					{
						pHead = pHead->next;
						--pCount;
					}
					else
					{
						// The second one is that we have a single item in the list
						// Equivalent to a clear
						pHead = NULL;
						pLast = NULL;
						pCount = 0;
					}
				}
				// Another victim
				++removeCount;
				// Destrying the item
				delete itemToDelete;
			}
			else
			{
				// This item does no interest us, let's continue
				previous = cursor;
				cursor = cursor->next;
			}
		} // while (cursor)
		return removeCount;
	}


	template<class T, class Alloc>
	typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::remove(reference_type value)
	{
		// !!! DO not forget to push changes into remove(const U&) as well

		// The current position in the list
		Item* cursor = pHead;
		// The previous item to 'cursor'
		Item* previous = NULL;
		Size removeCount = 0;

		// For each item in the list...
		while (cursor)
		{
			if (Private::LinkedListImpl::Compare<comparePointer>::Perform(value, cursor->data))
			{
				// We're keeping a reference to the item to remove
				Item* itemToDelete = cursor;
				// The previous valid item remains the same
				cursor = cursor->next;

				// Removing the item from the list
				if (previous)
				{
					// We have to remove the item in the middle of nowhere
					// We have to update the reference of the previous item
					previous->next = cursor; // the same than itemToDelete->next
					// The reference to the previous item may need an update
					if (pLast == itemToDelete)
						pLast = previous;
					--pCount;
				}
				else
				{
					// The item to remove is actually the first item
					// Two cases :
					// The first one we have more than one item in the list
					if (pLast != pHead)
					{
						pHead = pHead->next;
						--pCount;
					}
					else
					{
						// The second one is that we have a single item in the list
						// Equivalent to a clear
						pHead = NULL;
						pLast = NULL;
						pCount = 0;
					}
				}
				// Another victim
				++removeCount;
				// Destrying the item
				delete itemToDelete;
			}
			else
			{
				// This item does no interest us, let's continue
				previous = cursor;
				cursor = cursor->next;
			}
		} // while (cursor)
		return removeCount;
	}


	template<class T, class Alloc>
	template<class U, class A>
	typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::remove(const LinkedList<U,A>& list)
	{
		Item* cursor = list.pHead;
		typename LinkedList<T,Alloc>::Size removeCount = 0;
		while (cursor)
		{
			removeCount += this->remove(cursor->data);
			cursor = cursor->next;
		}
		return removeCount;
	}


	template<class T, class Alloc>
	template<class U, class A>
	typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::remove(const std::list<U,A>& rhs)
	{
		typename LinkedList<T,Alloc>::Size removeCount = 0;
		typedef std::list<U,A> L;

		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			removeCount += this->remove(*i);
		return removeCount;
	}


	template<class T, class Alloc>
	template<class U, class A>
	typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::remove(const std::vector<U,A>& rhs)
	{
		typename LinkedList<T,Alloc>::Size removeCount = 0;
		typedef std::vector<U,A> L;

		typename L::const_iterator end = rhs.end();
		for (typename L::const_iterator i = rhs.begin(); i != end; ++i)
			removeCount += this->remove(*i);
		return removeCount;
	}


	template<class T, class Alloc>
	void LinkedList<T,Alloc>::clear()
	{
		// Nothing to do if equals to 0
		if (pHead)
		{
			// Destroying all items
			doClear();
			// Resetting all internal variables
			pHead = NULL;
			pLast = NULL;
			pCount = 0;
		}
	}


	template<class T, class Alloc>
	inline bool LinkedList<T,Alloc>::empty() const
	{
		return (0 == pCount);
	}


	template<class T, class Alloc>
	inline typename LinkedList<T,Alloc>::Size LinkedList<T,Alloc>::size() const
	{
		return pCount;
	}


	template<class T, class Alloc>
	template<class U>
	inline LinkedList<T,Alloc>& LinkedList<T,Alloc>::operator += (const U& value)
	{
		push_back(value);
		return *this;
	}


	template<class T, class Alloc>
	template<class U>
	inline LinkedList<T,Alloc>& LinkedList<T,Alloc>::operator -= (const U& value)
	{
		(void) remove(value);
		return *this;
	}


	template<class T, class Alloc>
	template<class U>
	inline LinkedList<T,Alloc>& LinkedList<T,Alloc>::operator << (const U& value)
	{
		push_back(value);
		return *this;
	}

	template<class T, class Alloc>
	inline LinkedList<T,Alloc>& LinkedList<T,Alloc>::operator = (const LinkedList<T,Alloc>& value)
	{
		clear();
		push_back(value);
		return *this;
	}


	# ifdef YUNI_HAS_CPP_MOVE
	template<class T, class Alloc>
	inline LinkedList<T,Alloc>& LinkedList<T,Alloc>::operator = (LinkedList<T,Alloc>&& rhs)
	{
		pHead  = rhs.pHead;
		pLast  = rhs.pLast;
		pCount = rhs.pCount;
		rhs.pHead  = nullptr;
		rhs.pLast  = nullptr;
		rhs.pCount = 0;
		return *this;
	}
	# endif


	template<class T, class Alloc>
	inline bool LinkedList<T,Alloc>::operator ! () const
	{
		return (0 == pCount);
	}


	template<class T, class Alloc>
	void LinkedList<T,Alloc>::doClear()
	{
		// We will destroy each item in the list
		while (pHead)
		{
			// We can safely use pLast as a temporary variable
			pLast = pHead;
			pHead = pHead->next;
			delete pLast;
		}
	}



} // namespace Yuni
