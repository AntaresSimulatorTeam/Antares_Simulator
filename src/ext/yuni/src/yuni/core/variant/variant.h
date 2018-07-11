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
#include "../string.h"
#include "../static/remove.h"
#include "../static/if.h"
#include "fwd.h"
#include "types.h"
#include "dataholder/dataholder.h"
#include "dataholder/string.h"
#include "dataholder/array.h"



namespace Yuni
{

	/*!
	** \brief Variant type implementation.
	**
	** This is an implementation of Variants that handles
	** a limited range of types (classical ones), but that
	** can make automagic conversions between them, if such
	** a conversion is possible.
	**
	** It differs from Any, which can handle almost any type
	** (with the notable exception of arrays), but cannot
	** automatically convert from a type to another.
	**
	** How to use:
	** \code
	** Variant v = 12;
	**
	** // for direct conversions, without checking
	** std::cout << v.to<float>() << std::endl;
	** // with conversion checking
	** String s;
	** if (v.to(s))
	**	std::cout << s << std::endl;
	** else
	**	std::cout << "impossible to converter the variant into a string" << std::endl;
	** \endcode
	**
	** \ingroup Core
	*/
	class YUNI_DECL Variant final
	{
	public:
		/*!
		** \brief The most suitable smart ptr for this class
		**
		** This typedef is provided by the Yuni's conventions and should be useless
		** most of the time since reference counting is already used internally
		*/
		typedef SmartPtr<Variant> Ptr;

		template<class T>
		struct New
		{
			//! Instanciate a new variant
			static Variant Instance();
			//! Instanciate a new variant with array support
			static Variant Array();
		};

		//! Data holder
		typedef Private::Variant::IDataHolder  IDataHolder;

		template<class T>
		struct SupportedType
		{
			enum { yes = 0, no = 1 };
			//! Type used for operator overloading
			typedef SupportedType<T> OperatorType;
		};

	public:
		//! \name Constructors
		//@{
		//! Constructs an empty Variant
		Variant();
		//! Constructs a copy of an existing Variant.
		Variant(const Variant& rhs);
		//! Constructs from a dataholder
		Variant(const IDataHolder* rhs, bool ref = false);
		//! Constructs from a dataholder
		Variant(IDataHolder* rhs, bool ref = false);
		//! Constructor from nullptr
		Variant(const NullPtr&);

		//! Constructs a Variant based on an integer.
		Variant(sint32 rhs);
		//! Constructs a Variant based on an integer.
		Variant(sint64 rhs);
		//! Constructs a Variant based on an integer.
		Variant(uint32 rhs);
		//! Constructs a Variant based on an integer.
		Variant(uint64 rhs);
		//! Constructs a Variant based on an integer.
		Variant(float rhs);
		//! Constructs a Variant based on an integer.
		Variant(double rhs);
		//! Constructs a Variant based on a char.
		Variant(char rhs);
		//! Constructs a Variant based on a bool
		Variant(bool rhs);
		//! Constructs a Variant based on a string
		Variant(const char* rhs);
		//! Constructs a Variant based on a string
		template<uint ChunkT, bool ExpT> Variant(const CString<ChunkT, ExpT>& rhs);

		//! Destructor
		~Variant() {}
		//@}


		//! \name Alteration methods
		//@{
		//! Assignment from an existing Variant
		void assign(const Variant& rhs);
		//! Assignment from standard type (uint32)
		void assign(uint32 rhs);
		//! Assignment from standard type (sint32)
		void assign(sint32 rhs);
		//! Assignment from standard type (uint64)
		void assign(uint64 rhs);
		//! Assignment from standard type (sint64)
		void assign(sint64 rhs);
		//! Assignment from standard type (char)
		void assign(char rhs);
		//! Assignment from standard type (bool)
		void assign(bool rhs);
		//! Assignment from standard type (double)
		void assign(double rhs);
		//! Assignment from standard type (string)
		void assign(const String& rhs);
		//! Assignment from standard type (string)
		void assign(const char* rhs);

		//! operator + (variant)
		void add(const Variant& value);
		//! operator add (string)
		void add(const char* value);
		//! operator +
		template<class T> void add(const T& value);

		//! operator * (variant)
		void sub(const Variant& value);
		//! operator * (string)
		void sub(const char* value);
		//! operator *
		template<class T> void sub(const T& value);

		//! operator + (variant)
		void div(const Variant& value);
		//! operator add (string)
		void div(const char* value);
		//! operator +
		template<class T> void div(const T& value);

		//! operator * (variant)
		void mult(const Variant& value);
		//! operator * (string)
		void mult(const char* value);
		//! operator *
		template<class T> void mult(const T& value);

		//! Resets the Variant to an empty one.
		void clear();

		/*!
		** \brief Share content with another variant
		*/
		void shareContentFrom(const Variant& rhs);
		/*!
		** \brief Revoke reference sharing
		*/
		void shareContentFrom(const NullPtr&);
		//@}


		//! \name Information methods
		//@{
		/*!
		** \brief Get the type held by the inner value
		*/
		VariantInnerType type() const;
		/*!
		** \brief Returns true if the Variant is empty.
		*/
		bool empty() const;
		/*!
		** \brief Get if empty
		*/
		bool isnil() const;
		//@}


		//! \name Retrieval methods
		//@{
		/*!
		** \brief Retrieve the inner value into a given type
		** \return True if the conversion succeeded, false otherwise
		*/
		template<class T> bool to(T& out) const;

		/*!
		** \brief Retrieve the inner value into a given type
		*/
		template<class T> T to() const;
		//@}


		//! \name Operator overloads
		//@{
		//! operator =
		Variant&  operator = (const Variant& rhs);
		//! operator =
		template<class T> Variant&  operator = (const T& rhs);
		//! operator =
		Variant&  operator = (const IDataHolder* rhs);
		//! operator =
		Variant&  operator = (IDataHolder* rhs);
		//! operator nullptr
		Variant&  operator = (const NullPtr&);
		//! operator +=
		template<class T> Variant& operator += (const T& rhs);
		//! operator -=
		template<class T> Variant& operator -= (const T& rhs);
		//! operator *=
		template<class T> Variant& operator *= (const T& rhs);
		//! operator /=
		template<class T> Variant& operator /= (const T& rhs);
		//! operator []
		Variant operator [] (uint index);
		const Variant operator [] (uint index) const;

		//! Invoke method with no parameter
		Variant operator () (const String& method);
		//! Invoke method with 1 parameter
		Variant operator () (const String& method, const Variant& a1);
		//! Invoke method with 2 parameters
		Variant operator () (const String& method, const Variant& a1, const Variant& a2);
		//! Invoke method with 3 parameters
		Variant operator () (const String& method, const Variant& a1, const Variant& a2, const Variant& a3);
		//! Invoke method with 4 parameters
		Variant operator () (const String& method, const Variant& a1, const Variant& a2, const Variant& a3, const Variant& a4);

		//! Comparison
		bool operator == (const Variant& rhs) const;
		template<class U> bool operator == (const U& rhs) const;
		template<class U> bool operator != (const U& rhs) const;
		bool operator ! () const;
		//@}


	private:
		//! Perform a deep copy if the pointer is non unique
		void deepCopyIfNonUnique();

	private:
		//! Pointer to storage object
		Private::Variant::IDataHolder::Ptr pData;
		//! Flag to know if the content is currently shared
		bool pShareContent;

	}; // class Variant






} // namespace Yuni

#include "variant.hxx"
