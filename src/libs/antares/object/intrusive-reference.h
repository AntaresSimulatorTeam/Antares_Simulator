/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__
# define __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__

# include "../antares.h"
# include <yuni/core/smartptr.h>


namespace Antares
{

	/*!
	** \brief Give to inherited classes an intrusive counting through CRTP.
	**
	** \tparam ChildT Child class type
	** \tparam TP  Threading policy. Set by default for a single thread
	*/
	template<typename ChildT, template<class> class TP = Yuni::Policy::ObjectLevelLockable>
	class  IIntrusiveReference : public TP<IIntrusiveReference<ChildT,TP> >
	{
	public:
		//! Simplified type name of the instance for the current child type & threading policy.
		typedef IIntrusiveReference<ChildT,TP> IntrusiveRef;
		//! Threading policy type
		typedef TP<IntrusiveRef> ThreadingPolicy;

		/*!
		** \brief Class Helper to determine the most suitable smart pointer for a class
		**   according the current threading policy
		*/
		template<class T>
		class SmartPtr
		{
		public:
			//! A thread-safe type
			typedef Yuni::SmartPtr<T, Yuni::Policy::Ownership::COMReferenceCounted>  PtrThreadSafe;
			//! A default type
			typedef Yuni::SmartPtr<T, Yuni::Policy::Ownership::COMReferenceCounted>    PtrSingleThreaded;
			//! The most suitable smart pointer for T
			typedef typename Yuni::Static::If<ThreadingPolicy::threadSafe, PtrThreadSafe, PtrSingleThreaded>::ResultType Ptr;

		}; // class SmartPtr


	public:
		//! \name Pointer management
		//@{
		//! Increment the internal reference counter
		void addRef() const;
		//! Decrement the internal reference counter
		bool release() const;
		//! Get if the object is an unique reference (COW idiom)
		bool unique() const;
		//@}


		/*!
		** \brief Action to do when the release method is called
		**
		** For now, does nothing.
		** Called by children through static polymorphism (CRTP).
		** \attention Thread safe
		*/
		void onRelease();


	protected:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		IIntrusiveReference();

		/*!
		** \brief Destructor
		**
		** \internal The keyword 'virtual' is mandatory to have a proper call to the
		**   destructor
		*/
		virtual ~IIntrusiveReference();

		/*!
		** \brief Copy constructor
		** \param rhs Copy source
		*/
		explicit IIntrusiveReference(const IIntrusiveReference& rhs);
		//@}

		/*!
		** \brief Assignment operator
		** \param rhs Assignment source
		** \return Reference to "this"
		*/
		IIntrusiveReference& operator = (const IIntrusiveReference& rhs) const;

	private:
		typedef typename ThreadingPolicy::template Volatile<int>::Type ReferenceCounterType;
		//! Intrusive reference count
		mutable ReferenceCounterType pRefCount;

		// debug
		mutable std::list<std::list<YString> > pTraces;
		mutable std::list<std::list<YString> > pTracesFree;

	}; // class IIntrusiveReference






} // namespace Antares

# include "intrusive-reference.hxx"

#endif // __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__
