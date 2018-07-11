/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_H__
# define __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_H__

# include <yuni/yuni.h>
# include <yuni/string.h>
# include <yuni/core/foreach.h>
# include <vector>
# include <yuni/core/static/remove.h>
# include <algorithm>
# include <cassert>


namespace Antares
{
namespace Data
{

	template<class ItemT>
	class Equipments
	{
	public:
		//! The original equipment part
		typedef typename Yuni::Static::Remove::All<ItemT>::Type  ItemType;
		//! Weak pointer to an equipment part
		typedef ItemType*  ItemWeakPtr;
		//! Reference to an equipment part
		typedef ItemType&  ItemRef;
		//! Smart pointer
		typedef typename ItemType::Ptr  ItemPtr;

		//! Item vector
		typedef std::vector<ItemPtr>  ItemVector;
		//! Iterator
		typedef typename ItemVector::iterator iterator;
		//! Const Iterator
		typedef typename ItemVector::const_iterator const_iterator;


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Equipments();
		/*!
		** \brief Copy constructor
		*/
		Equipments(const Equipments& rhs);
		/*!
		** \brief Destructor
		**
		** The destructor is virtual to allow inheritance
		*/
		virtual ~Equipments();
		//@}


		//! \name Iterators
		//@{
		//! Begin
		iterator begin();
		//! Begin (const)
		const_iterator begin() const;
		//! Begin (const)
		const_iterator cbegin() const;

		//! End
		iterator end();
		//! End (const)
		const_iterator end() const;
		//! End (const)
		const_iterator cend() const;
		//@}


		//! \name Addition
		//@{
		/*!
		** \brief Try to add a new element
		*/
		template<class T> bool add(const T& element);
		/*!
		** \brief Try to add a new element
		*/
		bool add(const ItemType& element);
		//@}


		//! \name Removal
		//@{
		/*!
		** \brief Remove all elemets
		*/
		void clear();

		/*!
		** \brief Remove an element from its name
		** \return True if the element was removed
		*/
		uint remove(const AnyString& text);
		/*!
		** \brief Remove an element
		** \return True if the element was removed
		*/
		uint remove(const ItemPtr& element);
		/*!
		** \brief Remove an element
		** \return True if the element was removed
		*/
		uint remove(const ItemType& element);
		/*!
		** \brief Remove an element
		** \return True if the element was removed
		*/
		uint remove(const ItemType* element);

		/*!
		** \brief Remove an element from a given predicate
		** \return True if the element was removed
		*/
		template<class P> uint removeIf(const P& predicate);
		//@}


		//! \name Lookup
		//@{
		/*!
		** \brief Test the existence of an element from its name / id
		*/
		bool exists(const AnyString& text) const;
		/*!
		** \brief Test the existence of an element from its pointer
		*/
		bool exists(const ItemPtr& test) const;
		/*!
		** \brief Test the existence of an element from its pointer
		*/
		bool exists(const ItemType* test) const;
		/*!
		** \brief Test the existence of an element from its reference
		*/
		bool exists(const ItemType& test) const;

		/*!
		** \brief Try to find an element from its name or id
		*/
		ItemPtr find(const AnyString& text);

		/*!
		** \brief Try to find an element from its name or id
		*/
		ItemPtr find(const AnyString& text) const;

		/*!
		** \brief Iterate through all elements
		**
		** \code
		** equipments.each([&] (const Element& element)
		** {
		** 	std::cout << element.name() << std::endl;
		** });
		** \endcode
		*/
		template<class T> void each(const T& functor);

		/*!
		** \brief Try to find a name not already used
		*/
		template<class StringT>
		bool findNonExistingName(StringT& out, const AnyString& prefix, bool emptybefore = true) const;
		//@}


		//! \name Memory management
		//@{
		/*!
		** \brief Returns the number of elements
		*/
		uint size() const;
		/*!
		** \brief Get if the container is empty
		*/
		bool empty() const;

		/*!
		** \brief Reserve a change in capacity
		*/
		void reserve(uint count);

		/*!
		** \brief Begin update - avoid temporary all maintenance operations
		*/
		void beginUpdate();

		/*!
		** \brief End update - perform all maintenance operations
		*/
		void endUpdate();

		/*!
		** \brief ReIndex all items
		*/
		void reindex();

		/*!
		** \brief Get the amount of memory currently used by the container and all its elements
		*/
		Yuni::uint64 memoryUsage() const;
		//@}


		//! \name Operators
		//@{
		//! operator =
		Equipments&  operator = (const Equipments& rhs);
		//! operator []
		ItemType& operator [] (uint index);
		//! operator [] const
		const ItemType& operator [] (uint index) const;

		//! operator += - add
		template<class T> Equipments&  operator += (const T& element);
		//! operator -= - remove
		template<class T> Equipments&  operator -= (const T& element);
		//@}


	public:
		//! Flag to determine whether the container has been modified or not
		bool modified;


	private:
		//! Add a new element without checking if it is already present
		void addWithoutCheck(const ItemPtr& element);

	private:
		//! All items
		ItemVector pItems;
		//! Update counter
		uint pUpdateCounter;

	}; // class Equipments





} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_H__
