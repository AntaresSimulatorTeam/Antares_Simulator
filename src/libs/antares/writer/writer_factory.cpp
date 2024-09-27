/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include "antares/writer/writer_factory.h"

#include "antares/writer/in_memory_writer.h"

#include "private/immediate_file_writer.h"
#include "private/zip_writer.h"

namespace Antares::Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const std::filesystem::path& folderOutput,
                                       std::shared_ptr<Yuni::Job::QueueService> qs,
                                       Benchmarking::DurationCollector& duration_collector)
{
    using namespace Antares::Data;

    switch (fmt)
    {
    case zipArchive:
        return std::make_shared<ZipWriter>(qs, folderOutput, duration_collector);
    case inMemory:
        return std::make_shared<InMemoryWriter>(duration_collector);
    case legacyFilesDirectories:
    default:
        return std::make_shared<ImmediateFileResultWriter>(folderOutput.c_str());
    }
}
} // namespace Antares::Solver
