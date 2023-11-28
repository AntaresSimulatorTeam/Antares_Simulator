//
// Created by milos on 14/11/23.
//

#include "AuxillaryStructures.h"

namespace Antares::Solver::TSGenerator
{
// post-time step optimization - functions
int Maintenances::startDay(int limit) const
{
    // this functions finds first 1 in a vector and returns its index
    // if no 1, return -1
    // because 0 is a valid output - maintenance starts on first day

    auto vector = getStartSolutionValues();

    for (int i = 0; i < vector.size(); ++i)
    {
        if (vector[i] == 1)
        {
            if (i < limit)
            {
                return i;
            }
            else
            {
                break;
            }
        }
    }
    return -1;
}

std::vector<int> Maintenances::getStartSolutionValues() const
{
    std::vector<int> solutionValues;

    for (const auto& variable : start)
    {
        solutionValues.push_back(static_cast<int>(variable->solution_value()));
    }

    return solutionValues;
}

std::vector<std::array<double, DAYS_PER_YEAR>> Unit::availableDailyPower(int tsCount) const
{
    std::vector<std::array<double, DAYS_PER_YEAR>> result(tsCount,
                                                          std::array<double, DAYS_PER_YEAR>{});
    for (auto& arr : result)
    {
        arr.fill(100.0); // Set each array to the value 100.0
    }

    for (const auto& maintenance : maintenanceResults)
    {
        int start = maintenance.first;
        int duration = maintenance.second;

        int end = start + duration;
        if (end > DAYS_PER_YEAR)
            end = DAYS_PER_YEAR;

        for (int i = start; i < end; ++i)
        {
            result[0][i] = 0.0; // Assuming index is in range [0, 8760)
        }
    }
    return result;
}

} // namespace Antares::Solver::TSGenerator
