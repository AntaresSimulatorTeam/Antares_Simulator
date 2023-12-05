/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_LIB_OBJECT_H__
#define __ANTARES_LIB_OBJECT_H__

#include <yuni/string.h>
#include "ref.h"
#include <atomic>
#include <mutex>

namespace Antares
{
/*!
** \brief Base assembly abstract object
**
** \note An object is disabled by default to avoid race conditions
**  in future algorithms (race condtions while initializing for example)
*/
class IObject
{
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
    const Ref pOID;//!< Object Identifier
    YString pCaption;//!< Caption

    std::atomic_bool pEnabled=false;//!< If the object is enabled. Disabled by default
    mutable std::mutex mutex;

}; // class IObject

} // namespace Antares

#include "object.hxx"

#endif // __ANTARES_LIB_OBJECT_H__
