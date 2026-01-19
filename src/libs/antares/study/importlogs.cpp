// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <string>

#include <yuni/yuni.h>
#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;

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

    if (System::windows)
    {
        // On Windows, the access file is exclusive by default.
        // So we have to close the file before copying it.
        logs.closeLogfile();
    }

    resultWriter.addEntryFromFile("simulation.log", from.string());

    if (System::windows)
    {
        // Reopen the log file
        logs.logfile(from.string());
    }
}

} // namespace Antares::Data
