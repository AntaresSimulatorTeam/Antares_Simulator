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
#include "../exceptions/badcast.h"
#include "../string.h"
#include "any.private.h"



namespace Yuni
{

	/*!
	** \brief Container for values of any type.
	** \ingroup Any
	**
	** This is a relatively classic, yet (i hope) fast implementation of
	** an Any type.
	**
	** How to use:
	** \code
	** Any v(3.14);
	**
	** if (v.is<double>())
	**   // Be sure to check the type before, otherwise Any will throw an exception.
	**   std::cout << v.cast<double>();
	** \endcode
	*/
	class YUNI_DECL Any final
	{
	public:
		/*!
		** \brief Swaps an any with another
		** \param[in,out] one Any 1
		** \param[in,out] other Any 2
		*/
		static void Swap(Any& one, Any& other);


	public:
		//! \name Constructors
		//@{
		//! Copy of an existing variable
		template<class T> Any(const T& source);

		//! Copy of an existing const C string
		Any(const char* source)
		{ initFromCString(source); }

		//! Copy of an existing const C string
		Any(char* source)
		{ initFromCString(source); }

		//! Empty constructor
		Any();

		//! Copy constructor
		Any(const Any& rhs);

		//! Destructor
		~Any();
		//@}


		//! \name Alteration methods
		//@{

		/*!
		** \brief Assignment from another Any
		** \param[in] rhs The Any to assign from
		** \return This Any
		*/
		Any& assign(const Any& rhs);

		/*!
		** \brief Assignment from any object
		** \param[in] rhs The object to assign from
		** \return This Any
		*/
		template<class T>
		Any& assign(const T& rhs);

		/*!
		** \brief Specialized assign for C Strings.
		*/
		Any& assign(const char* rhs)
		{ return assign<String>(rhs); }

		/*!
		** \brief Assignment operator for convenience
		*/
		template<class T>
		Any& operator = (T const& rhs) {return assign(rhs);}

		/*!
		** \brief Resets the Any to an empty state.
		*/
		void reset();

		//@}

		//! \name Information methods
		//@{

		/*!
		** \brief Returns the type_info of the held variable.
		**
		** Can be used to compare with typeid(MyType).
		*/
		const std::type_info& type() const {return pTable->type();}


		/*!
		** \brief Returns true if the object is of the specified type
		*/
		template<class T>
		bool is() const {return type() == typeid(T);}


		/*!
		** \brief Checks if the any has been assigned a value.
		** \return True if the any contains no value.
		*/
		bool empty() const
		{ return pTable == Private::Any::Table<Private::Any::Empty>::Get(); }

		//@}

		//! \name Retrieval methods
		//@{

		/*!
		** \brief Casts the Any to the T type. Throws a Yuni::Exceptions::BadCast
		** if not possible.
		** \return A T object.
		*/
		template<class T>
		const T& to() const;

		//@}

	private:
		//! Special initializer for C Strings copys
		template<class T> void initFromCString(T source);

	private:
		//! Static function pointer table storage
		Private::Any::TypeManipulationTable * pTable;

		//! Object storage.
		void* pObject;
	};




} // namespace Yuni

#include "any.hxx"
