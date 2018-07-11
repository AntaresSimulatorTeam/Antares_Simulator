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
#include <list>
#include <vector>
#include "../static/remove.h"
#include "../static/types.h"
#include "../iterator.h"
#include "../noncopyable.h"


namespace Yuni
{

	/*!
	** \brief A singly linked list
	*/
	template<class T, class Alloc = None>
	class YUNI_DECL LinkedList final
	{
	public:
		//! Size type
		typedef uint Size;
		//! Size type (STL compliant)
		typedef Size size_type;

		//! Value type
		typedef T Type;
		//! Value type
		typedef T ValueType;
		//! Value type (STL compliant)
		typedef T value_type;
		//! Reference
		typedef T& reference_type;
		//! Reference (const)
		typedef const T& const_reference_type;
		//! Pointer
		typedef T* pointer_type;
		//! Pointer (const)
		typedef const T* const_pointer_type;

		// Forward declaration for iterators
		class Iterator;
		//! Iterator (STL compliant)
		typedef IIterator<Iterator, false> iterator;
		typedef IIterator<Iterator, true> const_iterator;

	protected:
		// Forward declaration
		class Item;

		enum
		{
			/*!
			** \brief Indicates whether to base the comparison between values on a dereferenced pointer
			**
			** \internal This check is used when the type T is a reference
			*/
			comparePointer = Static::Type::IsConst<T>::No && Static::Type::Compound<T>::IsReference,
		};

	public:
		# include "iterator.inc.hpp"

	public:
		//! \name Constructors & Destructor
		//@{
		//! Default Constructor
		LinkedList();
		//! Copy constructor
		LinkedList(const LinkedList& rhs);
		# ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		LinkedList(LinkedList&& rhs);
		# endif
		//! Copy constructor from another linked list
		template<class U, class A> LinkedList(const LinkedList<U,A>& rhs);
		//! Copy constructor from a standard std::list
		template<class U, class A> explicit LinkedList(const std::list<U,A>& rhs);
		//! Copy constructor from a standard std::vector
		template<class U, class A> explicit LinkedList(const std::vector<U,A>& rhs);
		//! Destructor
		~LinkedList();
		//@}


		//! \name Iterators & Accessors
		//@{
		/*!
		** \brief Get an iterator at the beginning of the list
		*/
		iterator begin();
		const_iterator begin() const;

		/*!
		** \brief Get an iterator at the end of the list
		*/
		iterator end();
		const_iterator end() const;

		//! Get the first item
		reference_type front();
		const_reference_type front() const;

		//! Get the last item
		reference_type back();
		const_reference_type back() const;
		//@}


		//! \name Find
		//@{
		/*!
		** \brief Get the first item equal to a given value
		**
		** \param value The value to look for
		** \return An iterator to the item found
		*/
		template<class U> iterator find(const U& value);
		iterator find(const_reference_type value);

		/*!
		** \brief Get the first item equal to a given value
		**
		** \param value The value to look for
		** \return An iterator to the item found
		*/
		template<class U> const_iterator find(const U& value) const;
		const_iterator find(const_reference_type value) const;
		//@}


		//! \name Add
		//@{
		/*!
		** \brief Add a value at the end of the list (default constructor)
		*/
		void push_back();
		/*!
		** \brief Add a value at the end of the list
		*/
		template<class U> void push_back(const U& value);
		void push_back(reference_type value);
		/*!
		** \brief Add a list of values at the end of the list
		*/
		template<class U, class A> void push_back(const LinkedList<U,A>& rhs);
		/*!
		** \brief Add a list of values at the end of the list
		*/
		template<class U, class A> void push_back(const std::list<U,A>& rhs);
		/*!
		** \brief Add a list of values at the end of the list
		*/
		template<class U, class A> void push_back(const std::vector<U,A>& rhs);

		/*!
		** \brief Add a value at the beginning of the list (default constructor)
		*/
		void push_front();
		/*!
		** \brief Add a value at the begining of the list
		*/
		template<class U> void push_front(const U& value);
		void push_front(reference_type value);

		/*!
		** \brief Add a list of values at the beginning of the list
		*/
		template<class U, class A> void push_front(const LinkedList<U,A>& rhs);
		/*!
		** \brief Add a list of values at the beginning of the list
		*/
		template<class U, class A> void push_front(const std::list<U,A>& rhs);
		/*!
		** \brief Add a list of values at the beginning of the list
		*/
		template<class U, class A> void push_front(const std::vector<U,A>& rhs);

		/*!
		** \brief Remove the first element
		*/
		void pop_front();
		//@}


		//! \name Remove
		//@{
		/*!
		** \brief Remove the item pointed to by the iterator
		*/
		void erase(iterator& i);

		/*!
		** \brief Remove any item equal to a given value
		**
		** \code
		** Yuni::LinkedList<int> list;
		** list.push_back(10);
		** list.push_back(42);
		** list.remove(42);
		** \endcode
		**
		** \code
		** struct PredicateRemove
		** {
		**		bool operator == (const int& rhs) const
		**		{
		**			return !(rhs % 2);
		**		}
		** };
		**
		** int main(void)
		** {
		**	Yuni::LinkedList<int>  list;
		**	list << 10 << 11 << 2 << 42;
		**	list.remove(PredicateRemove());
		**	return 0;
		** }
		** \endcode
		**
		** \param value The value to look for
		** \return The count of deleted items
		*/
		template<class U> Size remove(const U& value);
		Size remove(reference_type value);

		/*!
		** \brief Remove any item equal to a value in the given list
		**
		** \param rhs A list of values to remove
		** \return The number of items that have been removed
		*/
		template<class U, class A> Size remove(const LinkedList<U,A>& rhs);

		/*!
		** \brief Remove any item equal to a value in the given list
		**
		** \param rhs A list of values to remove
		** \return The number of items that have been removed
		*/
		template<class U, class A> Size remove(const std::list<U,A>& rhs);

		/*!
		** \brief Remove any item equal to a value in the given list
		**
		** \param rhs A list of values to remove
		** \return The number of items that have been removed
		*/
		template<class U, class A> Size remove(const std::vector<U,A>& rhs);

		/*!
		** \brief Remove all items at once
		*/
		void clear();
		//@}


		//! \name Memory
		//@{
		/*!
		** \brief Get if the container is empty
		** \return True if the container is empty, false otherwise
		*/
		bool empty() const;

		//! Get the number of items in the container
		Size size() const;
		//@}


		//! \name Operators
		//@{
		/*!
		** \brief Clear then add the value
		**
		** \param value The value to copy
		** \return Always *this
		*/
		LinkedList& operator = (const LinkedList& value);

		# ifdef YUNI_HAS_CPP_MOVE
		//! Move assignement
		LinkedList& operator = (LinkedList&& rhs);
		# endif

		/*!
		** \brief Append a new item at the end of the list
		**
		** \param value The value to copy and add
		** \return Always *this
		*/
		template<class U> LinkedList& operator += (const U& value);

		/*!
		** \brief Append a new item at the end of the list
		**
		** \param value The value to copy and add
		** \return Always *this
		*/
		template<class U> LinkedList& operator << (const U& value);

		/*!
		** \brief Remove all items equal to 'value'
		**
		** \param value The value to look for
		** \return Always *this
		*/
		template<class U> LinkedList& operator -= (const U& value);

		/*!
		** \brief Check if the container is empty
		**
		** This operator is strictly equivalent to the method 'empty()'
		** \return True if the container is empty, false otherwise
		*/
		bool operator ! () const;
		//@}


	protected:
		class Item final : private NonCopyable<Item>
		{
		public:
			Item() :
				next(nullptr)
			{}
			inline explicit Item(Item* n) :
				next(n), data()
			{}
			inline explicit Item(value_type& value) :
				next(nullptr), data(value)
			{}
			template<class U> inline explicit Item(const U& value) :
				next(nullptr), data(value)
			{}
			inline Item(Item* n, reference_type value) :
				next(n), data(value)
			{}
			template<class U> inline Item(Item* n, const U& value) :
				next(n), data(value)
			{}

		public:
			//! The next item in the list
			Item* next;
			//! The real value
			T data;

		}; // class Item

	protected:
		//! Internal clear (without resetting values)
		void doClear();

	protected:
		//! The head of the list
		Item* pHead;
		//! The last item in the list
		Item* pLast;
		//! Item count
		uint pCount;

	}; // class LinkedList





} // namespace Yuni

#include "slist.hxx"
