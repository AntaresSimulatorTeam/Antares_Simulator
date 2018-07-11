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
#include "event.h"



namespace Yuni
{

	template<class P>
	inline Event<P>::Event()
	{}


	template<class P>
	inline Event<P>::Event(const Event<P>& /*rhs*/)
	{}


	template<class P>
	inline Event<P>::~Event()
	{
		clearWL();
	}


	template<class P>
	inline uint Event<P>::size() const
	{
		return AncestorType::pBindList.size();
	}


	template<class P>
	void Event<P>::clearWL()
	{
		if (not AncestorType::pBindList.empty())
		{
			// We will inform all bound objects that we are no longer linked.
			{
				IEvent* baseThis = dynamic_cast<IEvent*>(this);

				const typename AncestorType::BindList::iterator end = AncestorType::pBindList.end();
				typename AncestorType::BindList::iterator i = AncestorType::pBindList.begin();
				for (; i != end; ++i)
				{
					if ((*i).isDescendantOfIEventObserverBase())
					{
						// Getting the object pointer, if any, then decrementing the ref counter
						const IEventObserverBase* base = ((*i).observerBaseObject());
						if (base)
							base->boundEventRemoveFromTable(baseThis);
					}
				}
			}
			// Clear our own list
			AncestorType::pBindList.clear();
		}
	}


	template<class P>
	inline void Event<P>::clear()
	{
		if (not AncestorType::pEmpty)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			// In this case, the flag `empty` must be set first, to avoid concurrent
			// calls to `invoke()` for nothing.
			AncestorType::pEmpty = true;
			// Cleanup !
			clearWL();
		}
	}


	template<class P>
	inline void Event<P>::connect(typename BindType::FunctionType pointer)
	{
		Bind<P> b;
		b.bind(pointer);

		typename ThreadingPolicy::MutexLocker locker(*this);
		# ifdef YUNI_HAS_CPP_MOVE
		AncestorType::pBindList.push_back(std::move(b));
		# else
		AncestorType::pBindList.push_back(b);
		# endif
		AncestorType::pEmpty = false;
	}


	template<class P>
	template<typename U>
	void Event<P>::connect(typename BindType::template WithUserData<U>::FunctionType pointer,
		typename BindType::template WithUserData<U>::ParameterType userdata)
	{
		Bind<P> b;
		b.bind(pointer, userdata);

		// locking
		typename ThreadingPolicy::MutexLocker locker(*this);
		# ifdef YUNI_HAS_CPP_MOVE
		AncestorType::pBindList.push_back(std::move(b));
		# else
		AncestorType::pBindList.push_back(b);
		# endif

		AncestorType::pEmpty = false;
		// unlocking
	}


	template<class P>
	template<class C>
	void Event<P>::connect(C* o, typename Event<P>::template PointerToMember<C>::Type method)
	{
		if (o)
		{
			Bind<P> b;
			b.bind(o, method);
			assert(b.isDescendantOfIEventObserverBase() and "Invalid class C. The calling class must inherit from Event::Observer<CRTP, ThreadingPolicy>");

			// Locking
			typename ThreadingPolicy::MutexLocker locker(*this);
			// list + increment ref counter
			# ifdef YUNI_HAS_CPP_MOVE
			AncestorType::pBindList.push_back(std::move(b));
			# else
			AncestorType::pBindList.push_back(b);
			# endif

			(dynamic_cast<const IEventObserverBase*>(o))->boundEventIncrementReference(dynamic_cast<IEvent*>(this));
			AncestorType::pEmpty = false;
			// Unlocking
		}
	}


	template<class P>
	template<class C>
	void Event<P>::connect(const C* o, typename Event<P>::template PointerToMember<C>::ConstType method)
	{
		if (o)
		{
			Bind<P> b;
			b.bind(o, method);
			assert(b.isDescendantOfIEventObserverBase() and "Invalid class C");

			// locking
			typename ThreadingPolicy::MutexLocker locker(*this);
			// list + increment ref counter
			# ifdef YUNI_HAS_CPP_MOVE
			AncestorType::pBindList.push_back(std::move(b));
			# else
			AncestorType::pBindList.push_back(b);
			# endif

			(dynamic_cast<const IEventObserverBase*>(o))->boundEventIncrementReference(dynamic_cast<IEvent*>(this));
			AncestorType::pEmpty = false;
			// unlocking
		}
	}


	template<class P>
	template<class U>
	void Event<P>::remove(const U* object)
	{
		if (object)
		{
			const IEventObserverBase* base = dynamic_cast<const IEventObserverBase*>(object);
			if (base)
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				if (not AncestorType::pBindList.empty())
				{
					typedef Yuni::Private::EventImpl::template
						PredicateRemoveObserverBase<typename AncestorType::BindType> RemoveType;
					AncestorType::pBindList.remove(RemoveType(dynamic_cast<IEvent*>(this), base));
					AncestorType::pEmpty = AncestorType::pBindList.empty();
				}
				// unlocking
			}
			else
			{
				typename ThreadingPolicy::MutexLocker locker(*this);
				if (not AncestorType::pBindList.empty())
				{
					typedef Yuni::Private::EventImpl::template
						PredicateRemoveObject<typename AncestorType::BindType> RemoveType;
					AncestorType::pBindList.remove(RemoveType(object));
					AncestorType::pEmpty = AncestorType::pBindList.empty();
				}
				// unlocking
			}
		}
	}


	template<class P>
	void Event<P>::unregisterObserver(const IEventObserverBase* pointer)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		// When unregistering an observer, we have to remove it without any more checks
		if (not AncestorType::pBindList.empty())
		{
			typedef Yuni::Private::EventImpl::template
				PredicateRemoveWithoutChecks<typename AncestorType::BindType> RemoveType;
			AncestorType::pBindList.remove(RemoveType(pointer));
		}
		AncestorType::pEmpty = AncestorType::pBindList.empty();
	}


	template<class P>
	inline Event<P>& Event<P>::operator = (const NullPtr&)
	{
		clear();
		return *this;
	}


	template<class P>
	inline Event<P>& Event<P>::operator = (const Event<P>& rhs)
	{
		AncestorType::assign(rhs);
		return *this;
	}


	template<class P>
	inline bool Event<P>::operator ! () const
	{
		return (AncestorType::pEmpty);
	}


	template<class P>
	inline bool Event<P>::empty() const
	{
		return (AncestorType::pEmpty);
	}





} // namespace Yuni
