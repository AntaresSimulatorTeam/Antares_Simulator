// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <sstream>

#include <antares/logs/logs.h>

namespace Antares::Solver
{
void WriteCommandLineIntoLogs(int argc, const char** argv)
{
    std::ostringstream buffer;
    for (int arg = 0; arg < argc; ++arg)
    {
        buffer << argv[arg];
        if (arg + 1 != argc)
        {
            buffer << " ";
        }
    }
    logs.info() << "  :: command: " << buffer.str();
}
} // namespace Antares::Solver
