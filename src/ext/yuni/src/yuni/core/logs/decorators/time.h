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
#include "../null.h"

namespace Yuni::Private::LogsDecorator
{
// Forward declarations
YUNI_DECL void WriteCurrentTimestampToBuffer(char buffer[32]);

} // namespace Yuni::Private::LogsDecorator

namespace Yuni::Logs
{
template<class LeftType = NullDecorator>
class YUNI_DECL Time: public LeftType
{
public:
    template<class Handler, class VerbosityType, class O>
    void internalDecoratorAddPrefix(O& out, const AnyString& s) const
    {
        out.put('[');

        char asc[32]; // MSDN specifies that the buffer length value must be >= 26 for validity
        Private::LogsDecorator::WriteCurrentTimestampToBuffer(asc);
        out.write(asc, 19);

        out.put(']');

        // Transmit the message to the next decorator
        LeftType::template internalDecoratorAddPrefix<Handler, VerbosityType, O>(out, s);
    }

}; // class Time

} // namespace Yuni::Logs
