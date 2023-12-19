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

    for (int day = 0; day < start.size(); ++day)
    {
        int value = static_cast<int>(start[day]->solution_value());
        if (value == 1)
        {
            if (day < limit)
            {
                return day;
            }
            else
            {
                break;
            }
        }
    }
    return -1;
}

void Unit::calculateAvailableDailyPower(int tsCount)
{
    int totalDays = tsCount * DAYS_PER_YEAR;
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

        for (int day = start; day < end; ++day)
        {
            availableDailyPower[day] = 0.0;
        }
    }
}

} // namespace Antares::Solver::TSGenerator
