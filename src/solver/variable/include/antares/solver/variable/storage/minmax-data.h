// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__

#include <cstdint>
#include <vector>

#include "antares/solver/variable/storage/intermediate.h"

namespace Antares::Solver::Variable::R::AllYears
{
class MinMaxData
{
public:
    struct Data
    {
        explicit Data(std::size_t n):
            values(n),
            indices(n)
        {
        }

        std::vector<double> values;
        std::vector<uint16_t> indices;
    };

    MinMaxData() = default;
    ~MinMaxData() = default;

    void resetInf();
    void resetSup();

    void mergeInf(uint year, const IntermediateValues& rhs);
    void mergeSup(uint year, const IntermediateValues& rhs);

    Data annual{1};
    Data monthly{MONTHS_PER_YEAR};
    Data weekly{WEEKS_PER_YEAR};
    Data daily{DAYS_PER_YEAR};
    Data hourly{HOURS_PER_YEAR};

}; // class MinMaxData

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_DATA_H__
