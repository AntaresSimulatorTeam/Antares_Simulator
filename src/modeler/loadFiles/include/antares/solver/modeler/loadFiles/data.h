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
#pragma once

#include <vector>

#include "library.h"
#include "system.h"

namespace Antares::Study::SystemModel
{

struct Data
{
    Data(const std::vector<Library>& libraries,
         std::unique_ptr<System> system,
         std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> dataSeries):
        libraries_(libraries),
        system_(std::move(system)),
        dataSeries_(std::move(dataSeries))
    {
    }

    const std::vector<Library>& getLibraries() const
    {
        return libraries_;
    }

    const System* getSystem() const
    {
        return system_.get();
    }

    const Optimisation::LinearProblemApi::ILinearProblemData* getDataSeries() const
    {
        return dataSeries_.get();
    }

private:
    std::vector<Library> libraries_;
    std::unique_ptr<System> system_;
    std::unique_ptr<Optimisation::LinearProblemApi::ILinearProblemData> dataSeries_;
};

} // namespace Antares::Study::SystemModel
