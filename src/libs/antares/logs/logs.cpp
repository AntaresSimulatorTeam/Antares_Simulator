/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/logs/logs.h"

namespace Antares
{
//! Our log facility
Yuni::Logs::Logger<LoggingHandlers, LoggingDecorators> logs;

} // namespace Antares

using namespace Antares;
using namespace Yuni;

int LogCompatibility(const char format[], ...)
{
    va_list parg;
    va_start(parg, format);
    Antares::logs.compatibility().vappendFormat(format, parg);
    va_end(parg);
    return 1;
}

void LogDisplayErrorInfos(uint errors, uint warnings, const char* message, bool printAsError)
{
    ShortString64 error;
    ShortString64 warning;

    switch (errors)
    {
    case 0:
        break;
    case 1:
        error = "1 error";
        break;
    default:
        error << errors << " errors";
        break;
    }
    switch (warnings)
    {
    case 0:
        break;
    case 1:
        warning = "1 warning";
        break;
    default:
        warning << warnings << " warnings";
        break;
    }

    auto logLambda = [&](auto&& stream)
    {
        if (errors and warnings)
        {
            stream << "Found " << error << " and " << warning << ": " << message;
        }
        else
        {
            if (errors)
            {
                stream << "Found " << error << ": " << message;
            }
            if (warnings)
            {
                stream << "Found " << warning << ": " << message;
            }
        }
    };

    if (printAsError)
    {
        logLambda(logs.error());
    }
    else
    {
        logLambda(logs.info());
    }
}
