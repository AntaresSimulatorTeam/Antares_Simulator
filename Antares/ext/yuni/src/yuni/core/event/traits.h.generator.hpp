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

#include "../slist/slist.h"
#include "../atomic/bool.h"



namespace Yuni
{
namespace Private
{
namespace EventImpl
{

	template<int N, class BindT> class WithNArguments;


	template<typename T>
	class EmptyPredicate final
	{
	public:
		typedef void ResultType;
	public:
		inline void operator () (T) {}
		static void result() {}
	};


	template<class BindT>
	class PredicateRemoveObject final
	{
	public:
		PredicateRemoveObject(const void* object) :
			pObject(object)
		{}

		bool operator == (const BindT& rhs) const
		{
			return  (pObject == reinterpret_cast<const void*>(rhs.object()));
		}
	private:
		const void* pObject;
	};


	template<class BindT>
	class PredicateRemoveObserverBase final
	{
	public:
		PredicateRemoveObserverBase(IEvent* event, const IEventObserverBase* object) :
			pEvent(event), pObject(object)
		{}

		bool operator == (const BindT& rhs) const
		{
			if (pObject == rhs.observerBaseObject())
			{
				pObject->boundEventDecrementReference(pEvent);
				return true;
			}
			return false;
		}
	private:
		IEvent* pEvent;
		const IEventObserverBase* pObject;
	};


	template<class BindT>
	class PredicateRemoveWithoutChecks final
	{
	public:
		PredicateRemoveWithoutChecks(const IEventObserverBase* object) :
			pObject(object)
		{}

		inline bool operator == (const BindT& rhs) const
		{
			return (rhs.isDescendantOf(pObject));
		}
	private:
		const IEventObserverBase* pObject;
	};



	struct FoldTypeVoid {};
	template<class T> struct FoldType { typedef T value_type; };
	template<> struct FoldType<void> { typedef FoldTypeVoid value_type; };




<% (0..generator.argumentCount).each do |i| %>
	template<class BindT>
	class WithNArguments<<%=i%>, BindT> : public Policy::ObjectLevelLockable<WithNArguments<<%=i%>,BindT> >
	{
	public:
		//! The Threading Policy
		typedef Policy::ObjectLevelLockable<WithNArguments<<%=i%>,BindT> > ThreadingPolicy;
		//! Bind
		typedef BindT BindType;
		//! The Return type
		typedef typename BindType::ReturnType R;<% (0..i-1).each do |j| %>
		//! Type of the argument <%=j%>
		typedef typename BindType::template Argument<<%=j%>>::Type A<%=j%>;<% end %>


	public:
		//! \name Constructors
		//@{
		//! Default constructor
		WithNArguments() :
			pEmpty(true)
		{}
		//! Copy constructor
		WithNArguments(const WithNArguments& rhs)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			pEmpty = rhs.pEmpty;
			pBindList = rhs.pBindList;
		}
		//@}

		//! \name Invoke
		//@{
		/*!
		** \brief Invoke the delegate
		*/
		void invoke(<%=generator.variableList(i)%>) const
		{
			if (not pEmpty)
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				const typename BindList::const_iterator end = pBindList.end();
				for (typename BindList::const_iterator i = pBindList.begin(); i != end; ++i)
					(*i).invoke(<%=generator.list(i,'a')%>);
			}
		}

		template<class CallbackT>
		R fold(typename FoldType<R>::value_type initval, const CallbackT& accumulator<%=generator.variableList(i, "A", "a", ", ")%>) const
		{
			if (not pEmpty)
			{
				typename FoldType<R>::value_type value = initval;
				typename ThreadingPolicy::MutexLocker locker(*this);
				const typename BindList::const_iterator end = pBindList.end();
				for (typename BindList::const_iterator i = pBindList.begin(); i != end; ++i)
					accumulator(value, (*i).invoke(<%=generator.list(i,'a')%>));
				return value;
			}
			return initval;
		}


		template<template<class> class PredicateT>
		typename PredicateT<R>::ResultType invoke(<%=generator.variableList(i)%>) const
		{
			PredicateT<R> predicate;
			if (not pEmpty)
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				const typename BindList::const_iterator end = pBindList.end();
				for (typename BindList::const_iterator i = pBindList.begin(); i != end; ++i)
					predicate((*i).invoke(<%=generator.list(i,'a')%>));
			}
			return predicate.result();
		}


		template<template<class> class PredicateT>
		typename PredicateT<R>::ResultType invoke(PredicateT<R>& predicate<%=generator.variableList(i, "A", "a", ", ")%>) const
		{
			if (not pEmpty)
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				const typename BindList::const_iterator end = pBindList.end();
				for (typename BindList::const_iterator i = pBindList.begin(); i != end; ++i)
					predicate((*i).invoke(<%=generator.list(i,'a')%>));
			}
			return predicate.result();
		}

		template<class EventT> void assign(EventT& rhs)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			typename ThreadingPolicy::MutexLocker lockerRHS(rhs);
			pBindList = rhs.pBindList;
			pEmpty = pBindList.empty();
		}

		/*!
		** \brief Invoke the delegate
		*/
		void operator () (<%=generator.variableList(i)%>) const
		{
			if (not pEmpty)
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				const typename BindList::const_iterator end = pBindList.end();
				for (typename BindList::const_iterator i = pBindList.begin(); i != end; ++i)
					(*i).invoke(<%=generator.list(i,'a')%>);
			}
		}
		//@}


	protected:
		//! Binding list (type)
		typedef LinkedList<BindType> BindList;
		//! A flag to know if the event is empty or not
		// This value must only set when the mutex is locked
		volatile bool pEmpty;
		//! Binding list
		BindList pBindList;
		// friend !
		template<class P> friend class Event;

	}; // class WithNArguments


<% end %>




} // namespace EventImpl
} // namespace Private
} // namespace Yuni
