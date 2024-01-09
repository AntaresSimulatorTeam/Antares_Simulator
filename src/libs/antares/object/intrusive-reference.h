/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__
#define __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__

#include "../antares.h"
#include <memory>

namespace Antares
{
/*!
** \brief Give to inherited classes an intrusive counting through CRTP.
**
** \tparam ChildT Child class type
** \tparam TP  Threading policy. Set by default for a single thread
*/
template<typename ChildT, template<class> class TP = Yuni::Policy::ObjectLevelLockable>
class IIntrusiveReference : public TP<IIntrusiveReference<ChildT, TP>>
{
public:
    //! Simplified type name of the instance for the current child type & threading policy.
    using IntrusiveRef = IIntrusiveReference<ChildT, TP>;
    //! Threading policy type
    using ThreadingPolicy = TP<IntrusiveRef>;

    /*!
    ** \brief Class Helper to determine the most suitable smart pointer for a class
    **   according the current threading policy
    */
    template<class T>
    class SmartPtr
    {
    public:
        //! A thread-safe type
        using PtrThreadSafe = Yuni::SmartPtr<T, Yuni::Policy::Ownership::COMReferenceCounted>;
        //! A default type
        using PtrSingleThreaded = Yuni::SmartPtr<T, Yuni::Policy::Ownership::COMReferenceCounted>;
        //! The most suitable smart pointer for T
        using Ptr = typename Yuni::Static::
          If<ThreadingPolicy::threadSafe, PtrThreadSafe, PtrSingleThreaded>::ResultType;
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
    IIntrusiveReference& operator=(const IIntrusiveReference& rhs) const;

private:
    using ReferenceCounterType = typename ThreadingPolicy::template Volatile<int>::Type;
    //! Intrusive reference count
    mutable ReferenceCounterType pRefCount;

    // debug
    mutable std::list<std::list<YString>> pTraces;
    mutable std::list<std::list<YString>> pTracesFree;

}; // class IIntrusiveReference

} // namespace Antares

#include "intrusive-reference.hxx"

#endif // __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_H__
