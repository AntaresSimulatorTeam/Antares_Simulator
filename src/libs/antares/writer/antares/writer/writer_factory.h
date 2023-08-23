#pragma once

#include <yuni/core/string.h>
#include <yuni/job/queue/service.h>

#include "i_writer.h"
#include "result_format.h"
#include <antares/benchmarking/info_collectors.h>

namespace Antares::Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const YString& folderOutput,
                                       std::shared_ptr<Yuni::Job::QueueService> qs,
                                       Benchmarking::IDurationCollector* duration_collector);
}

