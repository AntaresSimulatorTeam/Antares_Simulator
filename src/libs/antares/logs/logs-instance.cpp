// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logs.h"

#include <string>

// Kept in its own translation unit, separate from Logger/LogBuffer, so that pulling in the
// LogBuffer implementation from the static library does not also drag in this definition of
// Antares::logs. Test binaries that provide their own fake Antares::logs (see
// src/tests/src/libs/antares/logs) rely on that separation to avoid a duplicate symbol at link
// time.
namespace Antares
{
Logs::Logger logs;
}

void LogDisplayErrorInfos(unsigned int errors,
                          unsigned int warnings,
                          const char* message,
                          bool printAsError)
{
    using namespace Antares;

    std::string error;
    std::string warning;

    switch (errors)
    {
    case 0:
        break;
    case 1:
        error = "1 error";
        break;
    default:
        error = std::to_string(errors) + " errors";
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
        warning = std::to_string(warnings) + " warnings";
        break;
    }

    auto logLambda = [&](auto&& stream)
    {
        if (errors && warnings)
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
