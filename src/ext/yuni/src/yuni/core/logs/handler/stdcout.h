
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
#include <iostream>

#include "../null.h"

#if defined(YUNI_OS_WINDOWS)
#define YUNI_LOGS_COLORS_ALLOWED 1
#else
#define YUNI_LOGS_COLORS_ALLOWED 1
#endif

namespace Yuni::Logs
{
/*!
** \brief Log Handler: The standard output (cout & cerr)
*/
template<class NextHandler = NullHandler>
class YUNI_DECL StdCout: public NextHandler
{
public:
    enum Settings
    {
        colorsAllowed = YUNI_LOGS_COLORS_ALLOWED,
    };

    template<class LoggerT, class VerbosityType>
    void internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const
    {
#if defined(__GNUC__) && !defined(__clang__)
        using DecoratorsType = typename LoggerT::DecoratorsType;
#endif

        // Write the message to the std::cout/cerr
        if (VerbosityType::shouldUsesStdCerr)
        {
            logger.DecoratorsType::template internalDecoratorAddPrefix<StdCout, VerbosityType>(
              std::cerr,
              s);
            // Flush
            std::cerr << std::endl;
        }
        else
        {
            logger.DecoratorsType::template internalDecoratorAddPrefix<StdCout, VerbosityType>(
              std::cout,
              s);
            // Flush
            std::cout << std::endl;
        }

        // Transmit the message to the next handler
        NextHandler::template internalDecoratorWriteWL<LoggerT, VerbosityType>(logger, s);
    }

}; // class StdCout

} // namespace Yuni::Logs

#undef YUNI_LOGS_UNIX_COLORS_ALLOWED
