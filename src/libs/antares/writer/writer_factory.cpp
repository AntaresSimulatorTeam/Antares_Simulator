// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
        return std::make_shared<ImmediateFileResultWriter>(folderOutput);
    }
}
} // namespace Antares::Solver
