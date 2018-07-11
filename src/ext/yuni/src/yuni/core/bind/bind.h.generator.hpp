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
#include "../../yuni.h"
#include "../../thread/policy.h"
#include "../static/types.h"
#include "../static/assert.h"
#include "../static/remove.h"
#include "../smartptr.h"
#include "traits.h"
#include "../dynamiclibrary/symbol.h"
#include "fwd.h"


<%
require File.dirname(__FILE__) + '/../../../tools/generators/commons.rb'
generator = Generator.new()
%>
<%=generator.thisHeaderHasBeenGenerated("bind.h.generator.hpp")%>


namespace Yuni
{

	/*!
	** \brief A delegate implementation
	** \ingroup Events
	**
	** How to bind a mere function :
	** \code
	** #include <iostream>
	** #include <yuni/bind.h>
	**
	** static int Foo(int value)
	** {
	**	std::cout << "Foo: " << value << std::endl;
	**	return 0;
	** }
	**
	** int main()
	** {
	**	Yuni::Bind<int (int)> callback;
	**	callback.bind(&Foo);
	**	callback(42);
	**	callback(61);
	**	callback(-1)
	**	return 0;
	** }
	** \endcode
	**
	** How to bind a member of an object :
	** \code
	** #include <iostream>
	** #include <yuni/bind.h>
	**
	** class Foo
	** {
	** public:
	**	int bar(int value)
	**	{
	**		std::cout << "Foo::bar  : " << value << std::endl;
	**		return 0;
	**	}
	**	int bar2(int value)
	**	{
	**		std::cout << "Foo::bar2 : " << value << std::endl;
	**		return 0;
	**	}
	** };
	**
	** int main()
	** {
	**	Foo foo;
	**	Yuni::Bind<int (int)> callback;
	**	callback.bind(foo, &Foo::bar);
	**	callback(42);
	**	callback(61);
	**	callback(-1)
	**	callback.bind(foo, &Foo::bar2);
	**	callback(42);
	**	callback(61);
	**	callback(-1)
	**	return 0;
	** }
	** \endcode
	**
	** This class is thread-safe, this is guaranteed by the use of smartptr.
	**
	** \note This class does not take care of deleted objects. It is the responsibility
	** of the user to unbind the delegate before the linked object is delete and/or
	** to not invoke the delegate when the object does not exist.
	**
	** \note It is safe to provide a null pointer when binding the delegate
	** \note It is always safe to invoke the delegate when unbound.
	**
	** \tparam P The prototype of the targetted function/member
	*/
	template<class P = void (), class Dummy = void>
	class YUNI_DECL Bind;










	//
	// --- Specializations for Bind<> ---
	//


<%
(0..(generator.argumentCount)).each do |i|
[ ["class R" + generator.templateParameterList(i), "R ("+generator.list(i) + ")", "void"],
  ["class R" + generator.templateParameterList(i), "R (*)(" + generator.list(i) + ")", "void"],
  ["class ClassT, class R" + generator.templateParameterList(i), "R (ClassT::*)(" + generator.list(i) + ")", "ClassT"] ].each do |tmpl|
%>

	/*
	** \brief Bind to a function/member with <%=generator.xArgumentsToStr(i)%> (Specialization)
	*/
	template<<%=tmpl[0]%>>
	class YUNI_DECL Bind<<%=tmpl[1]%>, <%=tmpl[2]%>> final
	{
	public:
		//! The Bind Type
		typedef Bind<<%=tmpl[1]%>, <%=tmpl[2]%>> Type;
		//! The Bind Type
		typedef Bind<<%=tmpl[1]%>, <%=tmpl[2]%>> BindType;

<%=generator.include("yuni/core/bind/bind.h.generator.commonstypes.hpp", i) %>

	public:
		/*!
		** \brief Instanciate a Yuni::Bind from an exported symbol from a dynamic library
		** \param symbol A symbol from a shared library
		** \return A new bind
		*/
		static BindType FromSymbol(const Yuni::DynamicLibrary::Symbol& symbol);


	public:
		//! \name Constructor & Destructor
		//@{
		//! Default Constructor
		Bind();

		//! Copy constructor
		Bind(const Bind& rhs);
		# ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		Bind(Bind&& rhs);
		# endif

		/*!
		** \brief Constructor, from a pointer-to-method
		**
		** \param c A pointer to an object (can be null)
		** \param member A pointer-to-member
		*/
		template<class C> Bind(C* c, R (C::*member)(<%=generator.list(i)%>));

		# ifdef YUNI_HAS_CPP_BIND_LAMBDA
		/*!
		** \brief Constructor from a functor, most likely a lambda
		** \param functor A functor
		*/
		template<class C> Bind(C&& functor);
		# else
		/*!
		** \brief Constructor from a pointer-to-function
		** \param pointer A pointer-to-function
		*/
		Bind(R (*pointer)(<%=generator.list(i)%>));
		# endif
		//@}


		//! \name Bind
		//@{
		/*!
		** \brief Bind to a function
		**
		** \param pointer A pointer-to-function
		*/
		void bind(R (*pointer)(<%=generator.list(i)%>));

		/*!
		** \brief Bind to a function with a custom and persistent user data
		**
		** \tparam U The type of the user data
		** \param pointer  A pointer-to-function
		** \param userdata The userdata that will be copied and stored
		*/
		template<class U>
		void bind(R (*pointer)(<%=generator.list(i,'A', "", ", ")%>U), typename WithUserData<U>::ParameterType userdata);

		/*!
		** \brief Bind to an object member
		**
		** \tparam C Any class
		** \param c      A pointer to an object (can be null)
		** \param member A pointer-to-member
		*/
		template<class C> void bind(C* c, R (C::*member)(<%=generator.list(i)%>));

		/*!
		** \brief Bind to an object member
		**
		** \tparam C Any smartptr
		** \param c A pointer to an object (can be null)
		** \param member A pointer-to-member
		*/
		template<class C,
			template <class> class OwspP, template <class> class ChckP, class ConvP,
			template <class> class StorP, template <class> class ConsP>
		void bind(const SmartPtr<C, OwspP,ChckP,ConvP,StorP,ConsP>& c, R (C::*member)(<%=generator.list(i)%>));
		template<class C,
			template <class> class OwspP, template <class> class ChckP, class ConvP,
			template <class> class StorP, template <class> class ConsP>
		void bind(const SmartPtr<C, OwspP,ChckP,ConvP,StorP,ConsP>& c, R (C::*member)(<%=generator.list(i)%>) const);

		/*!
		** \brief Bind to a const object member
		**
		** \tparam C Any class
		** \param c      A pointer to an object (can be null)
		** \param member A pointer-to-member
		*/
		template<class C> void bind(const C* c, R (C::*member)(<%=generator.list(i)%>) const);

		/*!
		** \brief Bind to an object member with a custom and persistent user data
		**
		** \tparam U The type of the user data
		** \tparam C Any class
		** \param c        A pointer to an object (can be null)
		** \param member   A pointer-to-member
		** \param userdata User data
		*/
		template<class U, class C>
		void bind(C* c, R (C::*member)(<%=generator.list(i,'A', "", ", ")%>U), typename WithUserData<U>::ParameterType userdata);
		template<class U, class C>
		void bind(const C* c, R (C::*member)(<%=generator.list(i,'A', "", ", ")%>U) const, typename WithUserData<U>::ParameterType userdata);

		/*!
		** \brief Bind to an object member with a custom and persistent user data
		**
		** \tparam C Any smartptr
		** \param c        A pointer to an object (can be null)
		** \param member   A pointer-to-member
		** \param userdata User data
		*/
		template<class U, class C,
			template <class> class OwspP, template <class> class ChckP, class ConvP,
			template <class> class StorP, template <class> class ConsP>
		void bind(const SmartPtr<C, OwspP,ChckP,ConvP,StorP,ConsP>& c, R (C::*member)(<%=generator.list(i,'A', "", ", ")%>U),
			typename WithUserData<U>::ParameterType userdata);
		template<class U, class C,
			template <class> class OwspP, template <class> class ChckP, class ConvP,
			template <class> class StorP, template <class> class ConsP>
		void bind(const SmartPtr<C, OwspP,ChckP,ConvP,StorP,ConsP>& c, R (C::*member)(<%=generator.list(i,'A', "", ", ")%>U) const,
			typename WithUserData<U>::ParameterType userdata);


		/*!
		** \brief Bind from another Bind object
		*/
		void bind(const Bind& rhs);

		/*!
		** \brief Bind from a library symbol
		**
		** \param symbol A symbol from a dynamic library
		*/
		void bindFromSymbol(const Yuni::DynamicLibrary::Symbol& symbol);

		# ifdef YUNI_HAS_CPP_BIND_LAMBDA
		/*!
		** \brief Bind from a functor, most likely a lambda
		*/
		template<class C> void bind(C&& functor);
		# endif

		//@} // Bind


		//! \name Unbind
		//@{
		/*!
		** \brief Unbind
		**
		** It is safe to call this method several times
		*/
		void unbind();

		//! \see unbind
		void clear();

		//! Get if the object is binded
		bool empty() const;
		//@}


		//! \name Invoke
		//@{
		/*!
		** \brief Invoke the delegate
		**
		** The operator () can be used instead.
		*/
		R invoke(<%=generator.variableList(i)%>) const;

		/*!
		** \brief Invoke the bind using a getter for the arguments.
		**
		** Nothing will happen if the pointer is null
		** However, the returned value may not be what we shall expect
		** (the default constructor of the returned type is used in this case).
		**
		** \param userdata  Some user data
		*/
		template<class UserTypeT, template<class UserTypeGT, class ArgumentIndexTypeT> class ArgGetterT>
		R callWithArgumentGetter(UserTypeT userdata) const;
		//@}


		//! \name Print
		//@{
		/*!
		** \brief Print the value to the std::ostream
		**
		** \param out A stream used to write output
		*/
		void print(std::ostream& out) const;
		//@}


		//! \name Inheritance
		//@{
		/*!
		** \brief Get the raw pointer to the binded object (if any)
		**
		** If bound to a class, the return value will never be null. There is no way
		** to know statically the type of the object.
		** \warning It is the responsability to the user to use this method with care
		**
		** \return A non-null pointer if bound to a class
		*/
		const void* object() const;

		//! Get if the attached class is a descendant of 'IEventObserverBase'
		bool isDescendantOfIEventObserverBase() const;
		//! Get if the attached class is a real descendant of 'IEventObserverBase'
		bool isDescendantOf(const IEventObserverBase* obj) const;

		/*!
		** \brief Get the pointer to the binded object (if any) cast into IEventObserverBase
		**
		** \warning This method should never be used by the user
		** \return A non-null pointer if bound to a class
		*/
		const IEventObserverBase* observerBaseObject() const;
		//@}


		//! \name Operators
		//@{
		//! Get if the object is binded
		bool operator ! () const;
		/*!
		** \brief Invoke the delegate
		** \see invoke()
		*/
		R operator () (<%=generator.variableList(i)%>) const;
		//! Assignment with another Bind object
		Bind& operator = (const Bind& rhs);
		# ifdef YUNI_HAS_CPP_MOVE
		//! Assignment move
		Bind& operator = (Bind&& symbol);
		# endif
		//! Assignment with a pointer-to-function
		Bind& operator = (R (*pointer)(<%=generator.list(i)%>));
		//! Assignment with a library symbol
		Bind& operator = (const Yuni::DynamicLibrary::Symbol& symbol);
		# ifdef YUNI_HAS_CPP_BIND_LAMBDA
		//! Assignment from a functor, most likely a lambda
		template<class C> Bind& operator = (C&& functor);
		# endif

		//! Comparison with a pointer-to-function
		bool operator == (R (*pointer)(<%=generator.list(i)%>)) const;
		//! Comparison with a pointer-to-object
		template<class U> bool operator == (const U* object) const;
		//@}

	private:
		//! Empty callback when not binded (returns a default value)
		R emptyCallback(<%=generator.list(i)%>);
		//! Empty callback when not binded (returns void)
		void emptyCallbackReturnsVoid(<%=generator.list(i)%>);

	private:
		//! The holder type
		typedef Private::BindImpl::IPointer<R(<%=generator.list(i)%>)> IHolder;

		/*!
		** \brief Pointer to function/member
		** \internal The smartptr is used to guarantee the thread-safety, and to avoid
		** expensive copies
		*/
		SmartPtr<IHolder> pHolder;

		template<bool> friend struct Yuni::Private::BindImpl::MoveConstructor;

	}; // class Bind<R(<%=generator.list(i,'A')%>)>




<% end end %>







} // namespace Yuni

#include "bind.hxx"





template<class P, class DummyT>
inline std::ostream& operator << (std::ostream& out, const Yuni::Bind<P,DummyT>& rhs)
{
	rhs.print(out);
	return out;
}


// Comparison with any pointer-to-object
template<class U, class P, class DummyT>
inline bool operator == (const U* object, const Yuni::Bind<P,DummyT>& bind)
{
	return (bind == object);
}

// Comparison with any pointer-to-object
template<class U, class P, class DummyT>
inline bool operator != (const U* object, const Yuni::Bind<P,DummyT>& bind)
{
	return (bind != object);
}


<% (0..(generator.argumentCount)).each do |i| %>
template<class R, class P, class DummyT<%=generator.templateParameterList(i)%>>
inline bool operator == (R (*pointer)(<%=generator.list(i)%>), const Yuni::Bind<P,DummyT>& bind)
{
	return (bind == pointer);
}

template<class R, class P, class DummyT<%=generator.templateParameterList(i)%>>
inline bool operator != (R (*pointer)(<%=generator.list(i)%>), const Yuni::Bind<P,DummyT>& bind)
{
	return (bind != pointer);
}


<% end %>
