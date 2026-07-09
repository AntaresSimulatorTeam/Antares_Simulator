// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include "i_writer.h"
#include "result_format.h"

namespace Benchmarking
{
class DurationCollector;
}

namespace Antares::Concurrency
{
class ThreadPool;
}

namespace Antares::Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const std::filesystem::path& folderOutput,
                                       std::shared_ptr<Concurrency::ThreadPool> threadPool,
                                       Benchmarking::DurationCollector& duration_collector);
}
