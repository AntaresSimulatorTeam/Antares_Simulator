#pragma once

#include <yuni/core/string.h>
#include <yuni/job/queue/service.h>

#include <antares/study/fwd.h> // PowerFluctuations

#include "i_writer.h"
#include "immediate_file_writer.h"
#include "zip_writer.h"

namespace Antares
{
namespace Solver
{
IResultWriter::Ptr resultWriterFactory(Antares::Data::ResultFormat fmt,
                                       const YString& folderOutput,
                                       Yuni::Job::QueueService& qs);
}
} // namespace Antares
