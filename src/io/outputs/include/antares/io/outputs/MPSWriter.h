/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#pragma once

#include <filesystem>

#include "antares/optimisation/linear-problem-api/linearProblem.h"
namespace Antares::IO
{
class MPSWriter
{
public:
    explicit MPSWriter(const Antares::Optimisation::LinearProblemApi::ILinearProblem& lp,
                       const std::filesystem::path& path,
                       const std::string& name);
    void write();

private:
    const Optimisation::LinearProblemApi::ILinearProblem& linearProblem_;
    const std::string& name_;
    std::ofstream out_;
    //--//
    void writeHeader();
    void writeName();
    void writeRows();
    void writeColumns();
    void writeRhs();
    void writeBounds();
    void writeEnd();
};

} // namespace Antares::IO
