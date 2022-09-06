#ifndef __SOLVER_UTILS_STATISTICS_H__
#define __SOLVER_UTILS_STATISTICS_H__

#include <string>
#include <cmath>
#include <atomic>

class optimizationStatistics
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

    optimizationStatistics()
    {
        this->reset();
    }

    void add(const optimizationStatistics& other)
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

    unsigned int getNbSolve() const
    {
        return nbSolve;
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
            return 0.0;
        return ((double)totalUpdateTime) / nbUpdate;
    }

    double getAverageSolveTime() const
    {
        if (nbSolve == 0)
            return 0.0;
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