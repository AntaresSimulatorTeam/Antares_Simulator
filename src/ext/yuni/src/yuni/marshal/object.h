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
#include "../yuni.h"
#include "../core/string.h"



namespace Yuni
{
namespace Marshal
{

	/*!
	** \brief Base object for data serialization
	**
	** This class is a Variant-like structure, but less scalable and faster
	** for small data types.
	** Additionally, this class remains a POD type (no smart pointer or C++ advanced techniques)
	** for being used with malloc / calloc.
	*/
	class Object final
	{
	public:

		//! Internal type
		enum Type
		{
			// builtin types
			//! Nil object
			otNil = 0,
			//! String
			otString,
			//! Boolean
			otBool,
			//! Integer
			otInteger,
			//! Double,
			otDouble,
			// complex datatypes
			//! Array
			otArray,
			//! Dictionary
			otDictionary
		};

		// \internal for some reasons, the compiler requires to have this type public
		// (does not compile otherwise in object.cpp)
		union InternalDatatype
		{
			//! Value as integer
			sint64 integer;
			//! Value as bool
			bool boolean;
			//! Decimal
			double decimal;
			//! String
			String* string;
			//! Array
			void* array;
			//! Dictionary
			void* dictionary;
			//! blob
			void* blob;
		};


	public:
		//! \name Constructors & Destructor
		//@{
		Object();
		//! Copy constructor
		Object(const Object& rhs);
		#ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		Object(Object&&);
		#endif
		//! Constructor for internal operations
		Object(Type type, InternalDatatype value);
		//! Destructor
		~Object();
		//@}

		//! \name Clean
		//@{
		//! Clear internal variable
		void clear();
		//@}


		//! \name Assign
		//@{
		//! Copy operator
		void assign(const Object& rhs);
		//! assign boolean
		void assign(bool boolean);
		//! assign double
		void assign(double decimal);
		//! assign int
		void assign(int integer);
		//! assign int64
		void assign(sint64 integer);
		//! assign string
		void assign(const AnyString& string);
		//! assign cstring
		template<uint N> void assign(const char string[N]);
		//! assign cstring
		void assign(const char* string);
		//@}


		//! \name Append
		//@{
		//! Append an object
		void append(const Object& rhs);

		//! see append()
		template<class T> void push_back(const T& value);
		//@}


		//! \name Informations about internal data
		//@{
		//! Get the internal type
		Type type() const;

		/*!
		** \brief Get the number of item
		**
		** \return The number of items if an array or a dictionary, 0 if nil, 1 otherwise
		*/
		size_t size() const;

		/*!
		** \brief Swap the content of two object
		*/
		void swap(Object& second);
		//@}


		//! \name Import & Export
		//@{
		/*!
		** \brief Dump the content into a JSON structure
		**
		** \param out Stream output
		** \param pretty True to export in a pretty format (with spaces and indentation)
		*/
		void toJSON(Clob& out, bool pretty = true) const;
		//@}


		//! \name Operators
		//@{
		//! Copy operator
		Object& operator = (const Object& rhs);
		#ifdef YUNI_HAS_CPP_MOVE
		//! Move opertor
		Object& operator = (Object&&);
		#endif
		//! assign string
		template<uint N> Object& operator = (const char string[N]);
		//! assign something else
		template<class T> Object& operator = (const T& value);
		//! append
		template<class T> Object& operator += (const T& value);
		//! read/write the value of a given key
		Object& operator [] (const String& key);
		//@}


	private:
		template<bool PrettyT, class StreamT>
		void valueToJSON(StreamT& out, uint depth) const;

	private:
		//! Internal data type
		Type pType;
		//! Internal value
		InternalDatatype pValue;

	}; // class Object





} // namespace Marshal
} // namespace Yuni

#include "object.hxx"
