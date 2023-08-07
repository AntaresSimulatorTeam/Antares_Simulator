#include <antares/logs.h>
#include <antares/study.h>
#include "common.h"

using namespace Antares;

namespace Antares::Solver {

// weak_ptr because the lifetime must not be managed by this feature.
// If the study lifetime ends before we receive a signal, we shoud just do
// nothing when receiving the signal.
static std::weak_ptr<Antares::Data::Study> APPLICATION_STUDY;

void setApplicationStudy(std::weak_ptr<Antares::Data::Study> study)
{
    APPLICATION_STUDY = study;
}

}

namespace {

static void finalizeWrite() {
    auto study = Antares::Solver::APPLICATION_STUDY.lock();
    if (study) {
        auto writer = study->resultWriter;
        if (writer)
            writer->finalize(true);
        else {
            logs.warning() << "Could not finalize write: invalid writer";
            exit(EXIT_FAILURE);
        }
    } else {
        logs.warning() << "Could not finalize write: invalid study";
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
