// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>

#include <yuni/job/queue/service.h>

#include "i_writer.h"
#include "result_format.h"

namespace Benchmarking
{
class DurationCollector;
}

namespace Antares::Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const std::filesystem::path& folderOutput,
                                       std::shared_ptr<Yuni::Job::QueueService> qs,
                                       Benchmarking::DurationCollector& duration_collector);
}
