//
// Created by milos on 10/11/23.
//
#pragma once

#include "../../simulation/sim_extern_variables_globales.h"

using namespace Yuni;

#define SEP IO::Separator

#define FAILURE_RATE_EQ_1 0.999

namespace Antares::Solver::TSGenerator
{

class GeneratorTempData
{
    friend class OptimizationParameters;
    friend class OptimizedThermalGenerator;

public:
    GeneratorTempData(Data::Study& study, Solver::Progression::Task& progr, IResultWriter& writer);

    void prepareOutputFoldersForAllAreas(uint year);

    void operator()(Data::Area& area, Data::ThermalCluster& cluster);

public:
    Data::Study& study;

    bool archive;

    uint currentYear;

    uint nbThermalTimeseries;

    bool derated;

private:
    void writeResultsToDisk(const Data::Area& area, const Data::ThermalCluster& cluster);

    int durationGenerator(Data::ThermalLaw law, int expec, double volat, double a, double b);

    template<class T>
    void prepareIndispoFromLaw(Data::ThermalLaw law,
                               double volatility,
                               double A[],
                               double B[],
                               const T& duration);

private:
    const uint nbHoursPerYear = HOURS_PER_YEAR;
    const uint daysPerYear = DAYS_PER_YEAR;

    MersenneTwister& rndgenerator;

    double AVP[366];
    enum
    {
        Log_size = 4000
    };
    int LOG[Log_size];
    int LOGP[Log_size];

    double lf[366];
    double lp[366];
    double ff[366];
    double pp[366];
    double af[366];
    double ap[366];
    double bf[366];
    double bp[366];
    double FPOW[366][102];
    double PPOW[366][102];

    String pTempFilename;
    Solver::Progression::Task& pProgression;
    IResultWriter& pWriter;
};

} // namespace Antares::Solver::TSGenerator