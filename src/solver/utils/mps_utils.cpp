// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/utils/mps_utils.h"

#include <antares/study/study.h>
#include "antares/io/outputs/ISimulationTable.h"
#include "antares/io/outputs/MPSGenerator.h"
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;
using namespace Antares::Data;

/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#include <string>

#include "antares/solver/optimisation/opt_constants.h"

// ---------------------------------
// Full mps writing
// ---------------------------------
MPSwriter::MPSwriter(const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                     uint optNumber):
    I_MPS_writer(optNumber),
    linearProblem_(linearProblem)
{
}

void MPSwriter::runIfNeeded(Solver::IResultWriter& writer, const std::string& filename)
{
    // 0. Logging file name
    logs.info() << "Writing MPS File: `" << filename << "'";

    // 1. Write MPS
    auto mps = Antares::IO::Outputs::MPSGenerator(linearProblem_, filename).run();

    // 2. add the mps
    writer.addEntryFromBuffer(filename, mps);
}

mpsWriterFactory::mpsWriterFactory(
  Data::mpsExportStatus exportMPS,
  bool exportMPSOnError,
  const int current_optim_number,
  const Optimisation::LinearProblemApi::ILinearProblem& linearProblem):
    export_mps_(exportMPS),
    export_mps_on_error_(exportMPSOnError),
    linearProblem_(linearProblem),
    current_optim_number_(current_optim_number)
{
}

bool mpsWriterFactory::doWeExportMPS()
{
    switch (export_mps_)
    {
    case Data::mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return true;
    case Data::mpsExportStatus::EXPORT_FIRST_OPTIM:
        return current_optim_number_ == PREMIERE_OPTIMISATION;
    case Data::mpsExportStatus::EXPORT_SECOND_OPTIM:
        return current_optim_number_ == DEUXIEME_OPTIMISATION;
    default:
        return false;
    }
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::create()
{
    if (doWeExportMPS())
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createOnOptimizationError()
{
    if (export_mps_on_error_ || doWeExportMPS())
    {
        return createFullmpsWriter();
    }

    return std::make_unique<nullMPSwriter>();
}

std::unique_ptr<I_MPS_writer> mpsWriterFactory::createFullmpsWriter()
{
    return std::make_unique<MPSwriter>(linearProblem_, current_optim_number_);
}
