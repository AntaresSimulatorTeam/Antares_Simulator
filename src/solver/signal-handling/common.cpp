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
#include "antares/signal-handling/common.h"

#include <antares/logs/logs.h>
#include <antares/study/study.h>

using namespace Antares;

namespace Antares::Solver
{

// weak_ptr because the lifetime must not be managed by this feature.
// If the study lifetime ends before we receive a signal, we shoud just do
// nothing when receiving the signal.
static std::weak_ptr<IResultWriter> APPLICATION_WRITER;

void setApplicationResultWriter(std::weak_ptr<IResultWriter> writer)
{
    APPLICATION_WRITER = writer;
}

} // namespace Antares::Solver

namespace
{
void finalizeWrite()
{
    if (auto writer = Antares::Solver::APPLICATION_WRITER.lock())
    {
        writer->finalize(true);
    }
    else
    {
        logs.warning() << "Could not finalize write: invalid writer";
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

} // namespace

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
