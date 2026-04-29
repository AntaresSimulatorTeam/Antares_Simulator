// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

namespace Antares::Data
{
void Study::importLogsToOutputFolder(Solver::IResultWriter& resultWriter) const
{
    if (!logs.logfile())
    {
        return;
    }

    std::filesystem::path from = logs.logfile().c_str();
    from = from.lexically_normal();

#ifdef _WIN32
    // On Windows, the access file is exclusive by default.
    // So we have to close the file before copying it.
    logs.closeLogfile();
#endif

    resultWriter.addEntryFromFile("simulation.log", from.string());

#ifdef _WIN32
    // Reopen the log file
    logs.logfile(from.string());
#endif
}

} // namespace Antares::Data
