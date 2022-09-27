#include <antares/logs.h>
#include <antares/study.h>
#include "common.h"

using namespace Antares;

static void finalizeWrite()
{
    auto study = Data::Study::Current::Get();
    if (study)
    {
        auto writer = study->resultWriter;
        if (writer)
            writer->finalize(true);
        else
        {
            logs.warning() << "Could not finalize write: invalid writer";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        logs.warning() << "Could not finalize write: invalid study";
        exit(EXIT_FAILURE);
    }
     
    exit(EXIT_SUCCESS);
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
