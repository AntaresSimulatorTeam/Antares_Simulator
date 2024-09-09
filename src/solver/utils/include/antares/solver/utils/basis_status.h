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

#include <memory>

namespace Test
{
class BasisStatus;
}

namespace operations_research
{
class MPSolver;
}

namespace Antares::Optimization
{
class BasisStatusImpl;

class BasisStatus
{
public:
    // Prevent copy & move
    BasisStatus();
    ~BasisStatus();
    BasisStatus(const BasisStatus&) = delete;
    BasisStatus(BasisStatus&&) = delete;
    BasisStatus& operator=(const BasisStatus&) = delete;
    BasisStatus& operator=(BasisStatus&&) = delete;

    bool exists() const;
    void setStartingBasis(operations_research::MPSolver* solver) const;
    void extractBasis(const operations_research::MPSolver* solver);

private:
    std::unique_ptr<BasisStatusImpl> impl;
    friend class Test::BasisStatus; // For tests
};
} // namespace Antares::Optimization
