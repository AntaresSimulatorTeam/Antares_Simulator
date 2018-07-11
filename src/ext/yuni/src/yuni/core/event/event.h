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
#include "interfaces.h"
#include "../bind.h"
#include "../smartptr.h"
#include "traits.h"



namespace Yuni
{

	/*!
	** \brief Event
	** \ingroup Events
	*/
	template<class P = void ()>
	class YUNI_DECL Event final :
		public IEvent, // base class IEvent
		public Yuni::Private::EventImpl::WithNArguments<Bind<P>::argumentCount, Bind<P> >
	{
	public:
		//! Event
		typedef Event<P> EventType;
		//! The Bind Type
		typedef Bind<P> BindType;
		//! Type of the Ancestor
		typedef typename Yuni::Private::EventImpl::template WithNArguments<BindType::argumentCount, BindType> AncestorType;

		//! The Threading Policy
		typedef typename AncestorType::ThreadingPolicy ThreadingPolicy;
		//! The most suitable smartptr for the class
		typedef SmartPtr<Event> Ptr;
		//! Pointer-to-function
		typedef typename BindType::FunctionType FunctionType;

		/*!
		** \brief Pointer-to-member of any Class
		** \tparam C Any class
		*/
		template<class C>
		class PointerToMember final
		{
		public:
			//! Mere type for C
			typedef typename BindType::template PointerToMember<C>::Class Class;
			//! Type for a pointer-to-member
			typedef typename BindType::template PointerToMember<C>::Type Type;
			//! Type for a const pointer-to-member
			typedef typename BindType::template PointerToMember<C>::ConstType ConstType;
		};

		enum
		{
			//! The number of arguments
			argumentCount = BindType::argumentCount,
			//! A non-zero value if the prototype has a return value (!= void)
			hasReturnValue = BindType::hasReturnValue,
		};

		//! The type of the return value
		typedef typename BindType::ReturnType ReturnType;

		/*!
		** \brief Type of each argument by their index
		**
		** The typedef Type is always valid, even if the argument does not exist.
		** If this case, the type will be the empty class Yuni::None. You may
		** use `argumentCount` to know exactly the number of arguments.
		** \tparam I Index of the argument (zero-based)
		*/
		template<int I>
		struct Argument final
		{
			//! Type of the argument at position I (zero-based)
			typedef typename BindType::template Argument<I>::Type Type;
		};


	public:
		//! name Constructor & Destructor
		//@{
		//! Default Constructor
		Event();
		//! Copy constructor
		Event(const Event& rhs);
		//! Destructor
		~Event();
		//@}


		//! \name Connection
		//@{
		/*!
		** \brief Connect to a function
		**
		** \param pointer Pointer-to-function
		*/
		void connect(typename BindType::FunctionType pointer);

		/*!
		** \brief Connect to a function with a custom user data
		**
		** \param pointer Pointer-to-function
		** \param userdata The userdata to copy and store
		*/
		template<class U>
		void connect(typename BindType::template WithUserData<U>::FunctionType pointer,
			typename BindType::template WithUserData<U>::ParameterType userdata);

		/*!
		** \brief Connect to a object member
		**
		** \param o An object, with the class IEventObserver as ancestor (can be null)
		** \param method Pointer-to-member
		*/
		template<class C> void connect(C* o, typename PointerToMember<C>::Type method);
		/*!
		** \brief Connect to a const object member
		**
		** \param o An object, with the class IEventObserver as ancestor (can be null)
		** \param method const Pointer-to-member
		*/
		template<class C> void connect(const C* o, typename PointerToMember<C>::ConstType method);
		//@}


		//! \name Disconnection
		//@{
		/*!
		** \brief Disconnect all methods of an arbitrary object connected to the event
		**
		** \param object The object to find and to disconnect
		*/
		template<class U> void remove(const U* object);

		/*!
		** \brief Disconnect all
		*/
		void clear();
		//@}


		//! \name Misc
		//@{
		//! Get the number of current connections
		uint size() const;

		//! Get if the event does not have any single callback
		bool empty() const;
		//@}

		//[!] \name Invoke (documentation imported from the class WithNArguments)
		//@{
		// ReturnType invoke(<parameters>) const;
		// ReturnType operator () (<parameters>) const;
		//@}

		//! \name Operators
		//@{
		//! Assignment with a nullptr (equivalent to clear())
		Event& operator = (const NullPtr&);
		//! Copy operator
		Event& operator = (const Event& rhs);
		//! Get if the event is empty
		bool operator ! () const;
		//@}


	protected:
		/*!
		** \brief Unregister an observer
		**
		** This method will remove all delegates linked with the pointer to object.
		** This method should be called by any observer being destroyed.
		**
		** \param pointer Pointer-to-object (can be null)
		*/
		void unregisterObserver(const IEventObserverBase* pointer) override;

	private:
		//! Disconnect all callbacks (without lock)
		void clearWL();

	}; // class Event<>






} // namespace Yuni

#include "event.hxx"

