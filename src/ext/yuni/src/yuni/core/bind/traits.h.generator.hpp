/*
** YUNI's default license is the GNU Lesser Public License (LGPL), with some
** exclusions (see below). This basically means that you can get the full source
** code for nothing, so long as you adhere to a few rules.
**
** Under the LGPL you may use YUNI for any purpose you wish, and modify it if you
** require, as long as you:
**
** Pass on the (modified) YUNI source code with your software, with original
** copyrights intact :
**  * If you distribute electronically, the source can be a separate download
**    (either from your own site if you modified YUNI, or to the official YUNI
**    website if you used an unmodified version) – just include a link in your
**    documentation
**  * If you distribute physical media, the YUNI source that you used to build
**    your application should be included on that media
** Make it clear where you have customised it.
**
** In addition to the LGPL license text, the following exceptions / clarifications
** to the LGPL conditions apply to YUNI:
**
**  * Making modifications to YUNI configuration files, build scripts and
**    configuration headers such as yuni/platform.h in order to create a
**    customised build setup of YUNI with the otherwise unmodified source code,
**    does not constitute a derived work
**  * Building against YUNI headers which have inlined code does not constitute a
**    derived work
**  * Code which subclasses YUNI classes outside of the YUNI libraries does not
**    form a derived work
**  * Statically linking the YUNI libraries into a user application does not make
**    the user application a derived work.
**  * Using source code obsfucation on the YUNI source code when distributing it
**    is not permitted.
** As per the terms of the LGPL, a "derived work" is one for which you have to
** distribute source code for, so when the clauses above define something as not
** a derived work, it means you don't have to distribute source code for it.
** However, the original YUNI source code with all modifications must always be
** made available.
*/
#pragma once
<%
require File.dirname(__FILE__) + '/../../../tools/generators/commons.rb'
generator = Generator.new()
%>
<%=generator.thisHeaderHasBeenGenerated("traits.h.generator.hpp")%>
#include "../event/interfaces.h"
#include "../static/inherit.h"
#include "../static/dynamiccast.h"




namespace Yuni
{

	// Forward declaration
	template<typename P, class Dummy> class Bind;


} // namespace Yuni

namespace Yuni
{
namespace Private
{
namespace BindImpl
{


	// Forward declarations for classes which will hold informations about the
	// targetted function or member

	/*!
	** \brief Interface
	**
	** \tparam P The prototype of the function/member
	*/
	template<class P> class IPointer;

	/*!
	** \brief Dummy implementation, does nothing, used by unbind
	**
	** \tparam P The prototype of the function/member
	*/
	template<class P> class None;

	/*!
	** \brief Binding with a function
	**
	** \tparam P The prototype of the member
	*/
	template<class P> class BoundWithFunction;

	# ifdef YUNI_HAS_CPP_BIND_LAMBDA
	/*!
	** \brief Binding with a functor
	**
	** \tparam C The type of the functor
	** \tparam P The prototype of the member
	*/
	template<class C, class P> class BoundWithFunctor;
	# endif

	/*!
	** \brief Binding with a function
	**
	** \tparam U Type of the user data
	** \tparam P The prototype of the member
	*/
	template<class U, class P> class BoundWithFunctionAndUserData;

	/*!
	** \brief Binding with a member
	**
	** \tparam C Any class
	** \tparam P The prototype of the member
	*/
	template<class C, class P> class BoundWithMember;

	/*!
	** \brief Binding with a member and a user data
	**
	** \tparam U Type of the user data
	** \tparam C Any class
	** \tparam P The prototype of the member
	*/
	template<class U, class C, class P> class BoundWithMemberAndUserData;


	/*!
	** \brief Binding with a member
	**
	** \tparam C Any class
	** \tparam P The prototype of the member
	*/
	template<class C, class P> class BoundWithSmartPtrMember;

	/*!
	** \brief Binding with a member and a user data
	**
	** \tparam U Type of the user data
	** \tparam C Any class
	** \tparam P The prototype of the member
	*/
	template<class U, class C, class P> class BoundWithSmartPtrMemberAndUserData;





	/*!
	** \brief Helper to determine the type of an argument from its index
	**
	** \tparam P The prototype of a function/member
	** \tparam I The index of the argument (zero-based)
	*/
	template<class P, int I>
	struct Argument
	{
		// By default the argument does not exist, but we provide a valid type anyway
		typedef Yuni::None Type;
	};


	// There is no need for a partial specialization when the prototype does not
	// have any argument. `Yuni::None` will always be returned
<% (1..generator.argumentCount).each do |i| %>

	// Partial Specialization when the prototype has <%=generator.xArgumentsToStr(i)%>
<% (0..i-1).each do |j| %>	// Argument <%=j%>
	template<class R<%=generator.templateParameterList(i) %>>
	struct Argument<R(<%=generator.list(i)%>), <%=j%>> { typedef A<%=j%> Type; };
<% end end %>



	template<class T>
	struct Parameter
	{
		typedef const
			typename Static::Remove::Const<	typename Static::Remove::RefOnly<T>::Type>::Type
			& Type;
	};


	template<class T>
	struct Parameter<const T*>
	{
		typedef const T* Type;
	};

	template<class T>
	struct Parameter<T*>
	{
		typedef T* Type;
	};

	template<class T, int N>
	struct Parameter<const T[N]>
	{
		typedef const T* Type;
	};

	template<class T, int N>
	struct Parameter<T[N]>
	{
		typedef T* Type;
	};

	template<class T>
	struct Parameter<T&>
	{
		typedef const T& Type;
	};

	template<class T>
	struct Parameter<const T&>
	{
		typedef const T& Type;
	};







	// class IPointer
<% (0..generator.argumentCount-1).each do |i| %>
	template<class R<%=generator.templateParameterList(i) %>>
	class IPointer<R(<%=generator.list(i)%>)>
	{
	public:
		//! Destructor
		virtual ~IPointer() {}
		//! Invoke the delegate
		virtual R invoke(<%=generator.variableList(i)%>) const = 0;

		//! Get if the object is binded
		virtual bool empty() const { return false; }

		//! Get the pointer to object
		virtual const void* object() const = 0;

		//! Get the pointer to object cast into IEventObserverBase
		virtual const IEventObserverBase* observerBaseObject() const = 0;

		//! Get if the attached class is a descendant of 'IEventObserverBase'
		virtual bool isDescendantOf(const IEventObserverBase* obj) const = 0;

		//! Get if the attached class is a descendant of 'IEventObserverBase'
		virtual bool isDescendantOfIEventObserverBase() const = 0;

		//! Compare with a mere pointer-to-function
		virtual bool compareWithPointerToFunction(R (*pointer)(<%=generator.list(i)%>)) const = 0;
		//! Compare with a pointer-to-object
		virtual bool compareWithPointerToObject(const void* object) const = 0;

	};
<% end %>



	// class Void

<% (0..generator.argumentCount-1).each do |i| %>
	template<class R<%=generator.templateParameterList(i) %>>
	class None<R (<%=generator.list(i)%>)> final :
		public IPointer<R (<%=generator.list(i)%>)>
	{
	public:
		//! Destructor
		virtual ~None() {}


		virtual R invoke(<%=generator.list(i)%>) const override
		{
			return R();
		}

		virtual bool empty() const override
		{
			return true;
		}

		virtual const void* object() const override
		{
			return NULL;
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return NULL;
		}

		virtual bool isDescendantOf(const IEventObserverBase*) const override
		{
			return false;
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return false;
		}

		virtual bool compareWithPointerToFunction(R (*pointer)(<%=generator.list(i)%>)) const override
		{
			return (nullptr == pointer);
		}

		virtual bool compareWithPointerToObject(const void*) const override
		{
			return false;
		}

	}; // class None<R (<%=generator.list(i)%>)>
<% end %>



	// class BoundWithFunction

<% (0..generator.argumentCount-1).each do |i| %>
	template<class R<%=generator.templateParameterList(i) %>>
	class BoundWithFunction<R (<%=generator.list(i)%>)> final :
		public IPointer<R (<%=generator.list(i)%>)>
	{
	public:
		//! Destructor
		virtual ~BoundWithFunction() {}

		BoundWithFunction(R(*pointer)(<%=generator.list(i)%>)) :
			pPointer(pointer)
		{
			assert(pointer != nullptr and "binded pointer-to-function can not be null");
		}

		virtual R invoke(<%=generator.variableList(i)%>) const override
		{
			return (*pPointer)(<%=generator.list(i, 'a')%>);
		}

		virtual const void* object() const override
		{
			return NULL;
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return NULL;
		}

		virtual bool isDescendantOf(const IEventObserverBase*) const override
		{
			return false;
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return false;
		}

		virtual bool compareWithPointerToFunction(R (*pointer)(<%=generator.list(i)%>)) const override
		{
			return (reinterpret_cast<void*>(pPointer) == reinterpret_cast<void*>(pointer));
		}

		virtual bool compareWithPointerToObject(const void*) const override
		{
			return false;
		}


	private:
		//! Pointer-to-function
		R (*pPointer)(<%=generator.list(i)%>);

	}; // class BoundWithFunction<R (<%=generator.list(i)%>)>
<% end %>


	// class BoundWithFunctor

# ifdef YUNI_HAS_CPP_BIND_LAMBDA
<% (0..generator.argumentCount-1).each do |i| %>
	template<class C, class R<%=generator.templateParameterList(i) %>>
	class BoundWithFunctor<C, R (<%=generator.list(i)%>)> final :
		public IPointer<R (<%=generator.list(i)%>)>
	{
	public:
		//! Destructor
		virtual ~BoundWithFunctor() {}

		BoundWithFunctor(C&& functor) :
			pFunctor(std::forward<C>(functor))
		{}

		virtual R invoke(<%=generator.variableList(i)%>) const override
		{
			return pFunctor(<%=generator.list(i, 'a')%>);
		}

		virtual const void* object() const override
		{
			return nullptr;
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return nullptr;
		}

		virtual bool isDescendantOf(const IEventObserverBase*) const override
		{
			return false;
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return false;
		}

		virtual bool compareWithPointerToFunction(R (*pointer)(<%=generator.list(i)%>)) const override
		{
			return (reinterpret_cast<const void*>(&pFunctor) == reinterpret_cast<const void*>(pointer));
		}

		virtual bool compareWithPointerToObject(const void*) const override
		{
			return false;
		}


	private:
		//! Pointer-to-function
		C pFunctor;

	}; // class BoundWithFunctor<C, R (<%=generator.list(i)%>)>

<% end %>
# endif





	// class BoundWithFunctionAndUserData

<% (1..generator.argumentCount).each do |i| %>
	template<class U, class R<%=generator.templateParameterList(i) %>>
	class BoundWithFunctionAndUserData<U, R(<%=generator.list(i)%>)> final :
		public IPointer<R (<%=generator.list(i - 1)%>)>
	{
	public:
		BoundWithFunctionAndUserData(R(*pointer)(<%=generator.list(i)%>), U userdata)
			: pPointer(pointer), pUserdata(userdata)
		{
			assert(pointer != NULL and "binded pointer-to-function can not be null");
		}

		virtual R invoke(<%=generator.variableList(i-1)%>) const override
		{
			return (*pPointer)(<%=generator.list(i-1, 'a', "", ", ")%>*const_cast<UserDataTypeByCopy*>(&pUserdata));
		}

		virtual const void* object() const override
		{
			return NULL;
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return NULL;
		}

		virtual bool isDescendantOf(const IEventObserverBase*) const override
		{
			return false;
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return false;
		}

		virtual bool compareWithPointerToFunction(R (*pointer)(<%=generator.list(i-1)%>)) const override
		{
			return (reinterpret_cast<void*>(pPointer) == reinterpret_cast<void*>(pointer));
		}

		virtual bool compareWithPointerToObject(const void*) const override
		{
			return false;
		}


	private:
		//! Pointer-to-function
		R (*pPointer)(<%=generator.list(i)%>);
		//! Storage type
		typedef typename Static::Remove::RefOnly<A<%=i-1%>>::Type UserDataTypeByCopy;
		//! The user data
		UserDataTypeByCopy pUserdata;

	}; // class BoundWithFunctionAndUserData<U, R(<%=generator.list(i)%>)>


<% end %>







	// class BoundWithMember

<% (0..generator.argumentCount-1).each do |i| %>
	template<class C, class R<%=generator.templateParameterList(i) %>>
	class BoundWithMember<C, R(<%=generator.list(i)%>)> final :
		public IPointer<R(<%=generator.list(i)%>)>
	{
	public:
		//! \name Constructor
		//@{
		//! Constructor
		BoundWithMember(C* c, R(C::*member)(<%=generator.list(i)%>)) :
			pThis(c),
			pMember(member)
		{
			assert(c != NULL and "binded object can not be null");
		}
		//@}

		virtual R invoke(<%=generator.variableList(i)%>) const override
		{
			return (pThis->*pMember)(<%=generator.list(i, 'a')%>);
		}

		virtual const void* object() const override
		{
			return reinterpret_cast<void*>(pThis);
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::PerformConst(pThis);
		}

		virtual bool isDescendantOf(const IEventObserverBase* obj) const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Equals(obj, pThis);
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Yes;
		}

		virtual bool compareWithPointerToFunction(R (*)(<%=generator.list(i)%>)) const override
		{
			return false;
		}

		virtual bool compareWithPointerToObject(const void* object) const override
		{
			return (reinterpret_cast<const C*>(object) == pThis);
		}


	private:
		//! Pointer to the object
		mutable C* pThis;
		//! Pointer-to-member
		mutable R (C::*pMember)(<%=generator.list(i)%>);

	}; // class BoundWithMember<C, R(<%=generator.list(i)%>)>


<% end %>







	// class BoundWithMemberAndUserData

<% (1..generator.argumentCount).each do |i| %>
	template<class U, class C, class R<%=generator.templateParameterList(i) %>>
	class BoundWithMemberAndUserData<U, C, R(<%=generator.list(i)%>)> final :
		public IPointer<R(<%=generator.list(i-1)%>)>
	{
	public:
		typedef typename Static::Remove::RefOnly<A<%=i-1%>>::Type UserDataTypeByCopy;

	public:
		BoundWithMemberAndUserData(C* c, R(C::*member)(<%=generator.list(i)%>), U userdata) :
			pThis(c),
			pMember(member),
			pUserdata(userdata)
		{
			assert(c != NULL and "binded object can not be null");
		}

		virtual R invoke(<%=generator.variableList(i-1)%>) const override
		{
			return (pThis->*pMember)(<%=generator.list(i-1, 'a', "", ", ")%>const_cast<UserDataTypeByCopy&>(pUserdata));
		}

		virtual const void* object() const override
		{
			return reinterpret_cast<void*>(pThis);
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::PerformConst(pThis);
		}

		virtual bool isDescendantOf(const IEventObserverBase* obj) const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Equals(obj, pThis);
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Yes;
		}


		virtual bool compareWithPointerToFunction(R (*)(<%=generator.list(i-1)%>)) const override
		{
			return false;
		}

		virtual bool compareWithPointerToObject(const void* object) const override
		{
			return (reinterpret_cast<const C*>(object) == pThis);
		}


	private:
		//! Pointer to the object
		mutable C* pThis;
		//! Pointer-to-member
		mutable R (C::*pMember)(<%=generator.list(i)%>);
		//! Userdata
		UserDataTypeByCopy pUserdata;

	}; // class BoundWithMemberAndUserData<U, C, R(<%=generator.list(i)%>)>


<% end %>


<% (0..generator.argumentCount-1).each do |i| %>
	template<class PtrT, class R<%=generator.templateParameterList(i) %>>
	class BoundWithSmartPtrMember<PtrT, R(<%=generator.list(i)%>)> final :
		public IPointer<R(<%=generator.list(i)%>)>
	{
	public:
		typedef typename PtrT::Type  C;

	public:
		//! \name Constructor
		//@{
		//! Constructor
		BoundWithSmartPtrMember(const PtrT& c, R(C::*member)(<%=generator.list(i)%>)) :
			pThis(c),
			pMember(member)
		{}
		//@}

		virtual R invoke(<%=generator.variableList(i)%>) const override
		{
			return ((pThis.pointer())->*pMember)(<%=generator.list(i, 'a')%>);
		}

		virtual const void* object() const override
		{
			return reinterpret_cast<void*>(pThis.pointer());
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::PerformConst(pThis.pointer());
		}

		virtual bool isDescendantOf(const IEventObserverBase* obj) const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Equals(obj, pThis.pointer());
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Yes;
		}

		virtual bool compareWithPointerToFunction(R (*)(<%=generator.list(i)%>)) const override
		{
			return false;
		}

		virtual bool compareWithPointerToObject(const void* object) const override
		{
			return (reinterpret_cast<const C*>(object) == pThis.pointer());
		}


	private:
		//! Pointer to the object
		mutable PtrT pThis;
		//! Pointer-to-member
		mutable R (C::*pMember)(<%=generator.list(i)%>);

	}; // class BoundWithSmartPtrMember<C, R(<%=generator.list(i)%>)>


<% end %>







	// class BoundWithMemberAndUserData

<% (1..generator.argumentCount).each do |i| %>
	template<class U, class PtrT, class R<%=generator.templateParameterList(i) %>>
	class BoundWithSmartPtrMemberAndUserData<U, PtrT, R(<%=generator.list(i)%>)> final
		:public IPointer<R(<%=generator.list(i-1)%>)>
	{
	public:
		typedef typename PtrT::Type  C;
		typedef typename Static::Remove::RefOnly<A<%=i-1%>>::Type UserDataTypeByCopy;

	public:
		BoundWithSmartPtrMemberAndUserData(const PtrT& c, R(C::*member)(<%=generator.list(i)%>), U userdata) :
			pThis(c),
			pMember(member),
			pUserdata(userdata)
		{}

		virtual R invoke(<%=generator.variableList(i-1)%>) const override
		{
			return ((pThis.pointer())->*pMember)(<%=generator.list(i-1, 'a', "", ", ")%>const_cast<UserDataTypeByCopy&>(pUserdata));
		}

		virtual const void* object() const override
		{
			return reinterpret_cast<void*>(pThis.pointer());
		}

		virtual const IEventObserverBase* observerBaseObject() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::PerformConst(pThis.pointer());
		}

		virtual bool isDescendantOf(const IEventObserverBase* obj) const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Equals(obj, pThis.pointer());
		}

		virtual bool isDescendantOfIEventObserverBase() const override
		{
			return Static::DynamicCastWhenInherits<C,IEventObserverBase>::Yes;
		}


		virtual bool compareWithPointerToFunction(R (*)(<%=generator.list(i-1)%>)) const override
		{
			return false;
		}

		virtual bool compareWithPointerToObject(const void* object) const override
		{
			return (reinterpret_cast<const C*>(object) == pThis.pointer());
		}


	private:
		//! Pointer to the object
		mutable PtrT pThis;
		//! Pointer-to-member
		mutable R (C::*pMember)(<%=generator.list(i)%>);
		//! Userdata
		UserDataTypeByCopy pUserdata;

	}; // class BoundWithSmartPtrMemberAndUserData<U, C, R(<%=generator.list(i)%>)>


<% end %>







} // namespace BindImpl
} // namespace Private
} // namespace Yuni
