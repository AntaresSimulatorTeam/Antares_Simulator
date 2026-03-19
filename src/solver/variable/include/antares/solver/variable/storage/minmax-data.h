// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
        uint32_t index;
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
