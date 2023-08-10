#include <logs/logs.h>
#include <antares/study.h>
#include "common.h"

using namespace Antares;

namespace Antares::Solver {

// weak_ptr because the lifetime must not be managed by this feature.
// If the study lifetime ends before we receive a signal, we shoud just do
// nothing when receiving the signal.
static std::weak_ptr<IResultWriter> APPLICATION_WRITER;

void setApplicationResultWriter(std::weak_ptr<IResultWriter> writer)
{
    APPLICATION_WRITER = writer;
}

}

namespace {
void finalizeWrite() {
    if (auto writer = Antares::Solver::APPLICATION_WRITER.lock()) {
        writer->finalize(true);
    } else {
        logs.warning() << "Could not finalize write: invalid writer";
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

}

void signalCtrl_term(int)
{
    logs.notice() << "[signal] received signal SIGTERM. Exiting...";
    finalizeWrite();
}

void signalCtrl_int(int)
{
    logs.notice() << "[signal] received signal SIGINT. Exiting...";
    finalizeWrite();
}
