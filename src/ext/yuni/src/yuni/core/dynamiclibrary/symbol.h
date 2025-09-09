/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
/*!
** Header for Yuni::DynamicLibrary::Symbol, a class for representing an exported
** symbol by a (shared) library
*/
#include "../../yuni.h"

namespace Yuni::DynamicLibrary
{
/*!
** \brief Exported Symbol from a dynamic library
** \ingroup DynamicLibs
*/
class YUNI_DECL Symbol final
{
public:
    //! Handle for a symbol
    typedef void* Handle;

public:
    //! \name Constructor & Destructor
    //@{
    //! Default constructor
    Symbol();
    //! Constructor with a given handle
    Symbol(Handle p);
    //! Copy constructor
    Symbol(const Symbol& rhs);
    //@}

    //! \name Validity
    //@{
    //! Get if the symbol is invalid
    bool null() const;
    //! Get if the symbol is valid
    bool valid() const;
    //@}

    //! \name Operator
    //@{
    //! Copy operator
    Symbol& operator=(const Symbol& rhs);
    //! Copy operator
    Symbol& operator=(Symbol::Handle hndl);
    //@}

    //! \name Handle
    //@{
    /*!
    ** \brief Get the handle of the symbol
    */
    Handle ptr() const;
    //@}

private:
    //! Handle
    Handle pPtr;

}; // class Symbol

} // namespace Yuni::DynamicLibrary

#include "symbol.hxx"
