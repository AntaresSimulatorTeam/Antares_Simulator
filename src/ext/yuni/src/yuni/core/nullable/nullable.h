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
#include "../static/types.h"
#include "../static/remove.h"
#include "../static/moveconstructor.h"
#include "holder.h"
#include "../string.h"



namespace Yuni
{

	/*!
	** \brief A nullable value type
	**
	** This class behaves like the original type, except it can take the special
	** value `NULL`.
	**
	** It is safe to make operations (`+`, `+=`, `/`...) on this variable, even if
	** equals to NULL (a default value will be used).
	**
	** \code
	**	Yuni::Nullable<Yuni::String> s;
	**	std::cout << "After Init   :   s = " << s << std::endl;
	**
	**	s = "foo";
	**	std::cout << "After Assign :   s = " << s << std::endl;
	**
	**	// Reset to null
	**	s = nullptr;
	**	std::cout << "After reset  :   s = " << s << std::endl;
	** \endcode
	**
	** This class is a STL compliant container. Consequently, this class can be
	** used as it were a std::list or a std::vector, except that it can only contain
	** a single value in any cases.
	**
	** \note Accessing to methods of the class T: you have to use the operator `->`.
	** \code
	** Yuni::Nullable<Yuni::String> s;
	** std::cout << s->size() << std::endl;  // will display 0
	** s = "hello world";
	** std::cout << s->size() << std::endl;  // will display 11
	** \endcode
	**
	** \note <b>NULL</b> : `NULL` is defined as `0`, which is an int and not a pointer.
	** Consequently there is no way that code compiles without warnings, or does what
	** you expect :
	** \code
	** Yuni::Nullable<float> d;
	** d = NULL;
	** \endcode
	** Use instead the sepcial value `nullptr` or the method `clear()` :
	** \code
	** d = nullptr;
	** or
	** d.clear();
	** \endcode
	**
	** \tparam T The type
	** \tparam Alloc An allocator. Actually not used but here for compatibility with the STL
	*/
	template<class T, class Alloc = std::allocator<typename Static::Remove::Const<T>::Type> >
	class YUNI_DECL Nullable final
	{
	public:
		//! The type of object, T, stored in the container
		typedef typename Static::Remove::Const<T>::Type Type;

		//! Allocator
		typedef Alloc AllocatorType;
		//! Allocator
		typedef Alloc allocator_type;

		//! The type of object, T, stored in the container
		typedef Type value_type;
		//! Pointer to T
		typedef Type* pointer;
		//! Pointer to T
		typedef const Type* const_pointer;
		//! A reference
		typedef T& reference;
		//! A const reference
		typedef const T& const_reference;
		//! An unsigned integral type
		typedef typename AllocatorType::size_type size_type;
		//! A signed integral type
		typedef typename AllocatorType::difference_type difference_type;

		enum
		{
			//! Get if the type T is small or a fundamentl type
			typeIsSmall = Static::Type::IsSmall<Type>::Yes || Static::Type::IsFundamental<Type>::Yes
				|| Private::NullableImpl::IsNullable<T>::Yes,
		};

		//! The most suitable holder for T
		typedef Private::NullableImpl::Holder<Type, typeIsSmall> HolderType;

		template<bool Const>
		class iterator_base
		{
		public:
			iterator_base() {}

			bool valid() const {return true;}
		private:

		};

		//! Iterator
		typedef iterator_base<false> iterator;
		//! Const iterator
		typedef iterator_base<true>  const_iterator;
		//! Reverse iterator
		typedef iterator_base<false> reverse_iterator;
		//! Const Reverse iterator
		typedef iterator_base<true>  const_reverse_iterator;


	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Nullable();
		//! Copy constructor
		Nullable(const Nullable& rhs);
		//! Move constructor
		Nullable(Static::MoveConstructor<Nullable> rhs);
		//! Copy constructor
		template<class Alloc1> Nullable(const Nullable<T,Alloc1>& rhs);
		//! Constructor with a default value
		template<typename U> Nullable(const U& rhs);
		//! Constructor with a default value
		Nullable(const_pointer v);
		//! Destructor
		~Nullable();
		//@}


		//! \name Iterators
		//@{
		//! Returns an iterator pointing to the beginning of the vector
		iterator begin();
		//! Returns a const_iterator pointing to the beginning of the vector
		const_iterator begin() const;

		//! Returns a iterator pointing to the end of the vector
		iterator end();
		//! Returns a const_iterator pointing to the end of the vector
		const_iterator end() const;

		//! Returns a reverse_iterator pointing to the beginning of the reversed vector
		reverse_iterator rbegin();
		//! Returns a const_reverse_iterator pointing to the beginning of the reversed vector
		const_reverse_iterator rbegin() const;

		//! Returns a reverse_iterator pointing to the end of the reversed vector
		reverse_iterator rend();
		//! Returns a const_reverse_iterator pointing to the end of the reversed vector
		const_reverse_iterator rend() const;
		//@}


		//! Returns the size of the container (actually 0 or 1)
		size_type size() const;

		//! Returns the largest possible size of the container
		static size_type max_size();
		//! Returns the largest possible size of the container
		static size_type capacity();

		/*!
		** \brief Test if the container is empty
		*/
		bool empty() const;

		/*!
		** \brief Test if the variable is null
		*/
		bool null() const;


		//! Returns the n'th element
		reference operator[] (size_type n);
		//! Returns the n'th element
		const_reference operator[] (size_type n) const;

		reference front();
		reference back();
		const_reference front() const;
		const_reference back() const;

		/*!
		** \brief Reserve N elements
		**
		** Do nothing. Available for compatibility reasons only (STL).
		*/
		static void reserve(size_type N);


		/*!
		** \brief Insert a new element at the end
		**
		** The old value will be replaced by this new one
		*/
		void push_back(const_reference rhs);

		/*!
		** \brief Remove the last element (set the value to NULL)
		*/
		void pop_back();

		/*!
		** \brief Swap the content with another nullable object
		*/
		template<class Alloc1> void swap(Nullable<T,Alloc1>& rhs);

		iterator insert(iterator, const_reference x)
		{
			pHolder.assign(x);
			return begin();
		}

		iterator erase(iterator pos)
		{
			if (pos.valid())
				pHolder.clear();
			return end();
		}

		iterator erase(iterator first, iterator last)
		{
			if (first.valid() || last.valid())
				pHolder.clear();
			return end();
		}

		/*!
		** \brief Set the value to NULL
		*/
		void clear();

		/*!
		** \brief Inserts or erases elements at the end such that the size becomes n
		*/
		void resize(const size_type n, const T& t = T());


		//! \name Value
		//@{
		/*!
		** \brief Get the current value, or a default one if null
		*/
		Type value() const;
		/*!
		** \brief Get the current value, or a default one if null
		** \param nullValue The value to use the value is equal to null
		**
		** \code
		** Yuni::Nullable<Yuni::String> cellValue; // Null by default
		** Yuni::String strToDisplay = cellValue.value("(undef)");
		** std::cout << strToDisplay << std::endl; // will display `(undef)`
		** \endcode
		*/
		template<class U> Type value(const U& nullValue) const;
		//@}


		/*!
		** \brief Print the value to an stream
		**
		** The value will be print to the ostream using the operator <<.
		** If the value is null, `nullValue` will be used instead
		** \code
		** Yuni::Nullable<int> a;
		**
		** a = 42;
		** a.print(std::cout, "(null)");  // will print `42`
		** std::cout << std::endl;
		**
		** a = nullptr;
		** a.print(std::cout, "(null)");  // will print `(null)`
		** std::cout << std::endl;
		** \endcode
		*/
		template<class S, class U> void print(S& out, const U& nullValue) const;


		//! \name Operators
		//@{
		/*!
		** \brief Get a reference to the inner variable
		**
		** A default value will be created if equals to null
		*/
		reference operator * ();
		/*!
		** \brief Get a reference to the inner variable
		**
		** A default value will be created if equals to null
		*/
		reference operator -> ();


		//! Operator =
		Nullable& operator = (const Nullable& rhs);
		//! Operator =
		template<class Alloc1> Nullable& operator = (const Nullable<T,Alloc1>& rhs);
		//! Operator =
		Nullable& operator = (const NullPtr&);
		//! Operator =
		Nullable& operator = (const_pointer rhs);
		//! Operator =
		template<class U> Nullable& operator = (const U& rhs);

		//! Operator =
		Nullable& operator = (void* const);

		//! The comparison operator
		bool operator == (const Nullable& rhs) const;
		//! The comparison operator
		bool operator != (const Nullable& rhs) const;
		//! The comparison operator
		bool operator == (const NullPtr&) const;
		//! The comparison operator
		bool operator != (const NullPtr&) const;

		bool operator < (const Nullable& rhs) const;

		//! The operator !  (if (!value) ...)
		bool operator ! () const;
		//@}

	private:
		//! Empty class to allow  `if (mySmartPtr) ...`
		struct AutomaticConversion final
		{
			//! Disabling the delete operator
			void operator delete (void*);
		};

	public:
		//! \name Cast-Operator
		//@{
		//! Cast-Operator
		operator Static::MoveConstructor<Nullable> () {return Static::MoveConstructor<Nullable>(*this);}
		//! Cast-Operator for `if (a) ...`
		operator AutomaticConversion* () const
		{
			static AutomaticConversion tester;
			return (pHolder.empty()) ? NULL : &tester;
		}
		operator Type () const
		{
			return value();
		}
		//@}

	private:
		//! The dataholder
		HolderType pHolder;

	}; // class Nullable






} // namespace Yuni

#include "nullable.hxx"
#include "extension.hxx"

