#include <antares/logs.h>
#include <antares/study.h>
#include "common.h"

using namespace Antares;

void signalCtrl_term(int)
{
    logs.notice() << "[signal] received signal SIGTERM. Exiting...";
    auto study = Data::Study::Current::Get();
    if (study)
    {
        auto writer = study->getWriter();
        if (writer)
        {
            writer->finalize(true);
        }
    }
    exit(EXIT_SUCCESS);
}

void signalCtrl_int(int)
{
    logs.notice() << "[signal] received signal SIGINT. Exiting...";
    auto study = Data::Study::Current::Get();
    if (study)
    {
        auto writer = study->getWriter();
        if (writer)
        {
            writer->finalize(true);
        }
    }
    exit(EXIT_SUCCESS);
}
