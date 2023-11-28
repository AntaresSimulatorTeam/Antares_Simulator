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

void Unit::calculateAvailableDailyPower(int tsCount)
{
    int totalDays = tsCount * DAYS_PER_YEAR;
    // TODO CR27: ask Hugo. Do we take nom capacity or dailyPower. - and then later do mean for hourly values
    double maxPower = parentCluster->nominalCapacity;
    availableDailyPower.resize(totalDays);
    std::fill(availableDailyPower.begin(), availableDailyPower.end(), maxPower);

    for (const auto& maintenance : maintenanceResults)
    {
        int start = maintenance.first;
        int duration = maintenance.second;

        int end = start + duration;
        if (end > totalDays)
            end = totalDays;

        for (int mnt = start; mnt < end; ++mnt)
        {
            availableDailyPower[mnt] = 0.0;
        }
    }
}

} // namespace Antares::Solver::TSGenerator
