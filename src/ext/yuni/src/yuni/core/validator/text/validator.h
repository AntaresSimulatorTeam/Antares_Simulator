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
#ifndef __YUNI_CORE_VALIDATOR_VALIDATOR_H__
#define __YUNI_CORE_VALIDATOR_VALIDATOR_H__

#include "../../../yuni.h"
#include "../../string.h"
#include "../validator.h"

namespace Yuni::Validator::Text
{
template<class D>
class IValidatorTmpl
{
public:
    /*!
    ** \brief Perform a validation on any type of string
    **
    ** \param u An arbitrary string
    ** \return True if valid, false otherwise
    */
    template<class U>
    bool validate(const U& u) const
    {
        return static_cast<const D*>(this)->validate(u);
    }

    /*!
    ** \see validate()
    */
    template<class U>
    bool operator()(const U& u) const
    {
        return static_cast<const D*>(this)->validate(u);
    }

}; // class IValidatorTmpl

} // namespace Yuni::Validator::Text

#include "default.h"

#endif // __YUNI_CORE_VALIDATOR_VALIDATOR_H__
