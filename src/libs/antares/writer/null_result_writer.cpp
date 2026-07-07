// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/writer/null_result_writer.h>

namespace fs = std::filesystem;

namespace Antares::Solver
{

void NullResultWriter::addEntryFromBuffer(const fs::path&, std::string&)
{
}

void NullResultWriter::addEntryFromFile(const fs::path&, const fs::path&)
{
}

void NullResultWriter::flush()
{
}

bool NullResultWriter::needsTheJobQueue() const
{
    return false;
}

void NullResultWriter::finalize(bool)
{
}

} // namespace Antares::Solver
