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
#include <functional> // needed for the specialization of std::less
#include "policies/policies.h"
#include "../static/if.h"
#include "../static/moveconstructor.h"



namespace Yuni
{

	/*!
	** \brief Smart Pointer
	**
	** Smart pointers are abstract data type classes to simplify the memory management,
	** especially in a multithreaded context.
	** They simulate a pointer. They behave much like built-in C++ pointers except
	** that they automatically delete the object pointed to at the appropriate time
	** (automatic garbage collection), and can provide additional bounds checking.
	**
	** here is a simple example :
	** \code
	** #include <iostream>
	** #include <yuni/core/smartptr.h>
	**
	** struct Foo
	** {
	**		Foo(int v) : value(v) {}
	**		int value;
	** };
	**
	** int main(void)
	** {
	**		SmartPtr<Foo> ptr;
	**
	**		// Allocating a new `Foo` instance
	**		ptr = new Foo(42);
	**
	**		// Displaying its value, like if it were a standard pointer
	**		if (ptr)
	**			std::cout << "Value : " << ptr->value << std::endl;
	**
	**		// The instance will automatically be deleted when no needed
	**		return 0;
	** }
	** \endcode
	**
	**
	** Here is a more complex example, dealing with inheritance :
	** \code
	** #include <iostream>
	** #include <yuni/core/smartptr.h>
	**
	**
	** struct Foo
	** {
	**		Foo()
	**			:pValue(0)
	**		{
	**			std::cout << (void*)this << " Constructor" << std::endl;
	**		}
	**		Foo(const int v)
	**			:pValue(v)
	**		{
	**			std::cout << (void*)this << " Constructor, value = " << pValue << std::endl;
	**		}
	**		virtual ~Foo()
	**		{
	**			std::cout << (void*)this << " Destructor, value = " << pValue << std::endl;
	**		}
	**
	**		virtual void print() const {std::cout << "Foo : " << pValue << std::endl;}
	**
	** protected:
	**		int pValue;
	** };
	**
	** struct Bar : public Foo
	** {
	** public:
	**		Bar() {}
	**		Bar(const int v)
	**			:Foo(v)
	**		{}
	**		virtual Bar() {}
	**
	**		virtual void print() const {std::cout << "Bar : " << pValue << std::endl;}
	** };
	**
	** int main(void)
	** {
	**		Yuni::SmartPtr<Foo>  p1(new Foo(42));
	**		Yuni::SmartPtr<Bar>  p2(new Bar(72));
	**
	**		// The original pointer stored by p1 will no longer be needed anymore
	**		// and will be destroyed
	**		// For obvious reasons, we can not assign p2 to p1 (`Foo` is a superclass of `Bar`)
	**		p1 = p2;
	**
	**		// Print the values, p1 and p2 have the same pointer actually
	**		p1->print(); // 72
	**		p2->print(); // 72
	**
	**		// The original pointer stored in p2 will be destroyed here
	**		return 0;
	** }
	** \endcode
	**
	**
	** \note This smart pointer is thread-safe by default. If you do not need a locking
	**       mechanism, you should change the ownership policy.
	**
	** \warning Do not use SmartPtr with stack variables !
	** \code
	** #include <iostream>
	** #include <yuni/core/smartptr.h>
	**
	** struct Foo
	** {
	**		int bar;
	** };
	**
	** void doSomething(SmartPtr<Foo> p)
	** {
	**		std::cout << p->bar << std::endl; // Displays 42
	**
	**		// The pointer owned by `p` will be deleted at the end of the function
	** }
	**
	** int main()
	** {
	**		Foo foo;
	**		foo.bar = 42;
	**		doSomething(&foo);
	**
	**		// Foo will be also deleted here
	**		return 0;
	** }
	** \endcode
	**
	**
	** \tparam T      The type
	** \tparam OwspP  The ownership policy
	** \tparam ChckP  The Checking policy
	** \tparam ConvP  The Conversion policy
	** \tparam StorP  The Storage policy
	** \tparam ConsP  The Constness policy
	*/
	template <class T,                                                        // The original type
		template <class> class OwspP = Policy::Ownership::ReferenceCountedMT, // Ownership policy
		template <class> class ChckP = Policy::Checking::None,                // Checking policy
		class ConvP                  = Policy::Conversion::Allow,             // Conversion policy
		template <class> class StorP = Policy::Storage::Pointer,              // Storage policy
		template <class> class ConsP = Policy::Constness::DontPropagateConst  // Constness policy
		>
	class YUNI_DECL SmartPtr final :
		public StorP<T>,                              // inherits from the storage policy
		public OwspP<typename StorP<T>::PointerType>, // inherits from the ownership policy
		public ChckP<typename StorP<T>::StoredType>,  // inherits from the checking policy
		public ConvP                                  // inherits from the conversion policy
	{
	public:
		//! \name Type alias
		//@{
		//! Original type
		typedef T  Type;
		//! The Storage policy
		typedef StorP<T>  StoragePolicy;
		//! The Ownership policy
		typedef OwspP<typename StorP<T>::PointerType>  OwnershipPolicy;
		//! The conversion policy
		typedef ConvP  ConversionPolicy;
		//! The checking policy
		typedef ChckP<typename StorP<T>::StoredType>  CheckingPolicy;
		//! The Constness policy
		typedef ConsP<T>  ConstnessPolicy;

		//! the Pointer type
		typedef typename StoragePolicy::PointerType    PointerType;
		//! The Stored type
		typedef typename StoragePolicy::StoredType     StoredType;
		//! The Reference type
		typedef typename StoragePolicy::ReferenceType  ReferenceType;
		//! The Pointer type (const)
		typedef typename ConstnessPolicy::Type*        ConstPointerType;
		//! The Reference type (const)
		typedef typename ConstnessPolicy::Type&        ConstReferenceType;

		/*!
		** \brief Class Helper to determine the most suitable smart pointer for a class
		**   according the current threading policy
		*/
		template<class V>
		class Promote
		{
		public:
			//! A thread-safe type
			typedef Yuni::SmartPtr<V, OwspP, ChckP, ConvP, StorP, ConsP> Ptr;
		};

		//! Alias to itself
		typedef SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP> SmartPtrType;
		//! A smart pointer with a const pointer
		typedef SmartPtr<T,OwspP,ChckP,ConvP,StorP,Policy::Constness::PropagateConst> ConstSmartPtrType;
		//! A smart pointer with a non-const pointer
		typedef SmartPtr<T,OwspP,ChckP,ConvP,StorP,Policy::Constness::DontPropagateConst> NonConstSmartPtrType;

		//! The Type to use for the copy constructor
		typedef typename Static::If<OwnershipPolicy::destructiveCopy, SmartPtrType, const SmartPtrType>::ResultType  CopyType;

		//! Vector
		typedef std::vector<SmartPtrType>  Vector;
		//! List
		typedef std::list<SmartPtrType>  List;
		//@} // alias

	public:
		/*!
		** \brief Get the internal stored pointer (weak pointer) by the smart pointer
		**
		** \note This method should be used with care, especially in a multithreaded world
		** \param ptr The smart pointer
		** \return The internal stored pointer (can be null)
		*/
		static T* WeakPointer(const SmartPtr& ptr);
		/*!
		** \brief Get the internal stored pointer (weak pointer) by the smart pointer (identity)
		**
		** \note This method exists to avoid some common mistakes when mixing smartptr and raw pointers
		** \param wptr The pointer
		** \return The pointer wptr given in input
		*/
		static T* WeakPointer(T* wptr);

		/*!
		** \brief Perform a dynamic_cast on a smartptr
		**
		** Assuming the class `B` is a descendant of the class `A`
		** \code
		** A::Ptr a = new B();
		**
		** B::Ptr = A::Ptr::DynamicCast<B::Ptr>(a);
		** \endcode
		*/
		template<class S1> static S1 DynamicCast(SmartPtr p);

	private:
		/*!
		** \internal This class is used to provide a dummy type which will
		** produce an useless constructor with an improbable prototype.
		*/
	public:
		struct NeverMatched;
		//! Implicit - Conversions are allowed
		typedef typename Static::If< ConversionPolicy::allow, const StoredType&, NeverMatched>::ResultType ImplicitStoredType;
		//! Explicit - Conversions are not allowed
		typedef typename Static::If<!ConversionPolicy::allow, const StoredType&, NeverMatched>::ResultType ExplicitStoredType;


	public:
		//! \name Constructors & Destructor
		//@{
		//! Default constructor
		SmartPtr();

		/*!
		** \brief Constructor with a given pointer, when implicit types are not allowed
		*/
		explicit SmartPtr(ExplicitStoredType ptr);

		/*!
		** \brief Constructor with a given pointer, when implicit types are allowed
		*/
		SmartPtr(ImplicitStoredType ptr);

		/*!
		** \brief Constructor with a null pointer
		*/
		SmartPtr(const NullPtr&);

		/*!
		** \brief Copy constructor
		*/
		SmartPtr(CopyType& rhs);

		/*!
		** \brief Copy constructor
		*/
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr(const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs);

		/*!
		** \brief Copy constructor
		*/
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr(SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs);

		/*!
		** \brief Move Constructor
		*/
		SmartPtr(Static::MoveConstructor<SmartPtrType> rhs);

		//! Destructor
		~SmartPtr();
		//@}


		//! \name Swap
		//@{
		//! Swap from another smartptr
		void swap(SmartPtr& rhs);
		//@}


		//! \name Pointer
		//@{
		//! Get a pointer to the object, like operator -> would do
		PointerType pointer();
		//! Get a pointer to the object, like operator -> would do
		ConstPointerType pointer() const;
		//@}


		//! \name Operator * / ->
		//@{
		PointerType operator -> ();
		ConstPointerType operator -> () const;
		ReferenceType operator * ();
		ConstReferenceType operator * () const;
		//@}

		//! \name Operator =
		//@{
		//! Copy
		SmartPtr& operator = (CopyType& rhs);

		//! Reset
		SmartPtr& operator = (const NullPtr&);

		//! Copy from another smartptr
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr& operator = (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs);

		//! Copy from another smartptr
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr& operator = (SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs);
		//@}


		//! \name Comparisons
		//@{
		//! Operator `!` (to enable if (!mysmartptr) ...)
		bool operator ! () const;

		//! operator `==`
		bool operator == (const NullPtr&) const;
		//! operator `!=`
		bool operator != (const NullPtr&) const;

		//! operator `==`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator == (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;
		//! operator `==`
		bool operator == (const T* rhs) const;

		//! Operator `!=`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator != (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;
		//! Operator `!=`
		bool operator != (const T* rhs) const;

		//! Operator `<`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator < (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;

		//! Operator `>`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator > (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;

		//! Operator `<=`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator <= (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;

		//! Operator `>=`
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		bool operator >= (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const;
		//@}


	public:
		struct DynamicCastArg {};

		/*!
		** \brief Copy constructor with Dynamic casting
		*/
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr(const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs, const DynamicCastArg&);

		/*!
		** \brief Copy constructor with Dynamic casting
		*/
		template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
			class ConvP1,
			template <class> class StorP1, template <class> class ConsP1>
		SmartPtr(SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs, const DynamicCastArg&);

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
		operator Static::MoveConstructor<SmartPtr> () {return Static::MoveConstructor<SmartPtr>(*this);}
		//! Cast-Operator for `if (mySmartPtr) ...`
		operator AutomaticConversion* () const
		{
			static AutomaticConversion tester;
			return (!storagePointer(*this)) ? NULL : &tester;
		}
		//@}

	}; // class SmartPtr






} // namespace Yuni

#include "smartptr.hxx"
#include "stl.hxx"
