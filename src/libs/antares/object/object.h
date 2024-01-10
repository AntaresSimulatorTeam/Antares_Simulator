/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
*/
#ifndef __ANTARES_LIB_OBJECT_H__
#define __ANTARES_LIB_OBJECT_H__

#include "../antares.h"
#include <yuni/string.h>
#include <yuni/core/smartptr/intrusive.h>
#include "ref.h"
#include "intrusive-reference.h"
#include <atomic>

namespace Antares
{
/*!
** \brief Base assembly abstract object
**
** \note An object is disabled by default to avoid race conditions
**  in future algorithms (race condtions while initializing for example)
*/
class IObject : public Yuni::IIntrusiveSmartPtr<IObject>
{
public:
    //! Ancestor
    using Ancestor = Yuni::IIntrusiveSmartPtr<IObject>;
    //! The current threading policy
    using ThreadingPolicy = Ancestor::ThreadingPolicy;

    /*!
    ** \brief Class Helper to determine the most suitable smart pointer for a class
    **   according the current threading policy
    */
    template<class U>
    class SmartPtr
    {
    public:
        //! The most suitable smart pointer for T
        using Ptr = typename Ancestor::template SmartPtrType<U>::Ptr;
    };

    //! The most suitable smart pointer for the this class
    using Ptr = Ancestor::SmartPtrType<IObject>::Ptr;

public:
    //! \name Identifiers
    //@{
    /*!
    ** \brief Object Identifier
    */
    const Ref& oid() const;
    //@}

    //! \name Caption
    //@{
    /*!
    ** \brief Get the object's caption
    ** \return Object's caption
    */
    YString caption() const;
    /*!
    ** \brief Set the caption field
    ** \param caption String to use as caption.
    */
    void caption(const AnyString& caption);
    //@}

    //! \name Enabled
    //@{
    /*!
    ** \brief Get the object's state
    ** \return Object's state
    */
    bool enabled() const;
    /*!
    ** \brief Set the object stated (enabled or not)
    ** \param state State to set
    */
    void enabled(bool state);
    //@}

    //! \name Events
    //@{
    //! The object is about to be destroyed
    virtual void onRelease() const;
    //@}

protected:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Initialize the new IObject with the given UOID.
    */
    IObject();
    /*!
    ** \brief Initialize the new IObject with the given assembly name.
    */
    explicit IObject(const Ref& oid);
    /*!
    ** \brief Destructor
    */
    virtual ~IObject();
    //@}

protected:
    //! The most suitable type for a bool
    using EnableType
      = Yuni::Static::If<ThreadingPolicy::threadSafe, std::atomic<int>, bool>::ResultType;

    //! Object Identifier
    const Ref pOID;
    //! Caption
    YString pCaption;
    //! If the object is enabled. Disabled by default
    EnableType pEnabled;

}; // class IObject

} // namespace Antares

#include "object.hxx"

#endif // __ANTARES_LIB_OBJECT_H__
