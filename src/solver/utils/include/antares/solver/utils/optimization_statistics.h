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
#ifndef __SOLVER_UTILS_STATISTICS_H__
#define __SOLVER_UTILS_STATISTICS_H__

#include <atomic>
#include <cmath>
#include <string>

class OptimizationStatistics
{
private:
    std::atomic<long long> totalSolveTime;
    std::atomic<unsigned int> nbSolve;

    std::atomic<long long> totalUpdateTime;
    std::atomic<unsigned int> nbUpdate;

public:
    void reset()
    {
        totalSolveTime = 0;
        nbSolve = 0;
        totalUpdateTime = 0;
        nbUpdate = 0;
    }

    OptimizationStatistics()
    {
        this->reset();
    }

    OptimizationStatistics(OptimizationStatistics&& rhs):
        totalSolveTime(rhs.totalSolveTime.load()),
        nbSolve(rhs.nbSolve.load()),
        totalUpdateTime(rhs.totalUpdateTime.load()),
        nbUpdate(rhs.nbUpdate.load())
    {
    }

    OptimizationStatistics(const OptimizationStatistics&) = delete;
    OptimizationStatistics& operator=(const OptimizationStatistics&) = delete;
    OptimizationStatistics& operator=(OptimizationStatistics&&) = delete;

    void add(const OptimizationStatistics& other)
    {
        totalSolveTime += other.totalSolveTime;
        totalUpdateTime += other.totalUpdateTime;
        nbSolve += other.nbSolve;
        nbUpdate += other.nbUpdate;
    }

    void addUpdateTime(long long updateTime)
    {
        totalUpdateTime += updateTime;
        nbUpdate++;
    }

    void addSolveTime(long long solveTime)
    {
        totalSolveTime += solveTime;
        nbSolve++;
    }

    unsigned int getNbUpdate() const
    {
        return nbUpdate;
    }

    long long getTotalSolveTime() const
    {
        return totalSolveTime;
    }

    long long getTotalUpdateTime() const
    {
        return totalUpdateTime;
    }

    double getAverageUpdateTime() const
    {
        if (nbUpdate == 0)
        {
            return 0.0;
        }
        return ((double)totalUpdateTime) / nbUpdate;
    }

    double getAverageSolveTime() const
    {
        if (nbSolve == 0)
        {
            return 0.0;
        }
        return ((double)totalSolveTime) / nbSolve;
    }

    std::string toString() const
    {
        return "Average solve time: " + std::to_string(std::lround(getAverageSolveTime())) + " ms, "
               + "average update time: " + std::to_string(std::lround(getAverageUpdateTime()))
               + " ms";
    }
};

#endif
