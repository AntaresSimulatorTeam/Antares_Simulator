
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

namespace Yuni::Logs
{
template<class NextHandler>
bool File<NextHandler>::logfile(const AnyString& filename)
{
    // Assigning the new filename
    pOutputFilename = filename;
    // Opening the log file
    if (pOutputFilename.empty())
    {
        pFile.close();
        return true;
    }
    return pFile.open(pOutputFilename, IO::OpenMode::write | IO::OpenMode::append);
}

template<class NextHandler>
inline void File<NextHandler>::closeLogfile()
{
    pFile.close();
}

template<class NextHandler>
inline bool File<NextHandler>::reopenLogfile()
{
    pFile.close();
    return (pOutputFilename.empty())
             ? true
             : pFile.open(pOutputFilename, IO::OpenMode::write | IO::OpenMode::append);
}

template<class NextHandler>
inline String File<NextHandler>::logfile() const
{
    return pOutputFilename;
}

template<class NextHandler>
inline bool File<NextHandler>::logfileIsOpened() const
{
    return (pFile.opened());
}

template<class NextHandler>
template<class LoggerT, class VerbosityType>
void File<NextHandler>::internalDecoratorWriteWL(LoggerT& logger, const AnyString& s) const
{
    if (pFile.opened())
    {
#if defined(__GNUC__) && !defined(__clang__)
        using DecoratorsType = typename LoggerT::DecoratorsType;
#endif
        // Append the message to the file
        logger.DecoratorsType::template internalDecoratorAddPrefix<File, VerbosityType>(pFile, s);

// Flushing the result
#ifdef YUNI_OS_WINDOWS
        pFile << "\r\n";
#else
        pFile << '\n';
#endif
        pFile.flush();
    }

    // Transmit the message to the next handler
    NextHandler::template internalDecoratorWriteWL<LoggerT, VerbosityType>(logger, s);
}

} // namespace Yuni::Logs
