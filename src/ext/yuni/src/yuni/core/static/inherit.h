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
#include "../../yuni.h"

namespace Yuni::Static
{
/*!
** \brief Statically check if an arbitrary class C is a descendant of another one
**
** \code
** #include <yuni/static/inherit.h>
** #include <iostream>
**
** class Base {};
**
** class A : public Base {};
**
** class B {};
**
** int main()
** {
**	std::cout << "A inherits from Base :" << Yuni::Static::InheritsFrom<A,Base>::Yes << "\n";
**	std::cout << "B inherits from Base :" << Yuni::Static::InheritsFrom<B,Base>::Yes << "\n";
**	return 0;
** }
** \endcode
**
** \tparam C Any class (may be virtual)
** \tparam BaseT The base class to compare with (may be virtual)
*/
template<class C, class BaseT>
class InheritsFrom final
{
private:
    typedef int TrueType;
    typedef char FalseType;

    // C obviously inherits from BaseT
    static TrueType deduce(const BaseT*);
    // C does not inherits from BaseT
    static FalseType deduce(...);
    // A dummy routine, for returning a type C
    static C* Helper();

public:
    enum
    {
        //! A non-zero value if C inherits from BaseT
        Yes = sizeof(deduce(Helper())) == sizeof(TrueType),
        //! A non-zero value if C does not inherit from BaseT
        No = sizeof(deduce(Helper())) == sizeof(FalseType),
    };
}; // class InheritsFrom

} // namespace Yuni::Static
