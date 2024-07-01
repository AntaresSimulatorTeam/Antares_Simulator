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
#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__

#include <vector>

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
class MinMaxData
{
public:
    struct Data
    {
        double value;
        uint32_t indice;
    };

    MinMaxData() = default;
    ~MinMaxData() = default;

    void resetInf();
    void resetSup();

    void mergeInf(uint year, const IntermediateValues& rhs);
    void mergeSup(uint year, const IntermediateValues& rhs);

    std::vector<Data> annual{1};
    std::vector<Data> monthly{MONTHS_PER_YEAR};
    std::vector<Data> weekly{WEEKS_PER_YEAR};
    std::vector<Data> daily{DAYS_PER_YEAR};
    std::vector<Data> hourly{HOURS_PER_YEAR};

}; // class MinMaxData

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__
