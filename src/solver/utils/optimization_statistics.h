#ifndef __SOLVER_UTILS_STATISTICS_H__
#define __SOLVER_UTILS_STATISTICS_H__

#include <string>

class optimizationStatistics
{
private:
    long long totalSolveTime;
    unsigned int nbSolve;

    long long totalUpdateTime;
    unsigned int nbUpdate;

public:
    optimizationStatistics() :
        totalSolveTime(0),
        nbSolve(0),
        totalUpdateTime(0),
        nbUpdate(0)
    {
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
        if (nbUpdate == 0) return 0.0;
        return ((double) totalUpdateTime) / nbUpdate;
    }

    double getAverageSolveTime() const 
    {
        if (nbSolve == 0) return 0.0;
        return ((double) totalSolveTime) / nbSolve;
    }
};

namespace std {
    inline std::string to_string(const optimizationStatistics& os) 
    {
        return "average solve time: " + std::to_string(os.getAverageSolveTime()) + "ms, " 
            + "average update time: " + std::to_string(os.getAverageUpdateTime()) + "ms";
    }
}

#endif