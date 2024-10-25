#pragma once

#include "../state.h"

namespace Antares::Solver::Variable::Economy
{

struct MaxMRGinput
{
    double *spillage = nullptr;
    double *dens = nullptr;
    double *hydroGeneration = nullptr;
    double *hydroMaxPower = nullptr;
    double *dtgMargin = nullptr;
    unsigned int hourInYear = 0;
    Date::Calendar *calendar = nullptr;
    std::string areaName;
};

MaxMRGinput dataToComputeMaxMRG(const State &state, unsigned int numSpace);
void computeMaxMRG(double *opmrg, const MaxMRGinput &in);

} // Antares::Solver::Variable::Economy

