#pragma once

#include <antares/series/series.h>

#include "../state.h"

namespace Antares::Solver::Variable::Economy
{

struct MaxMRGinput
{
    double* spillage = nullptr;
    double* dens = nullptr;
    double* hydroGeneration = nullptr;
    Antares::Data::TimeSeries* maxHourlyGenPower = nullptr;
    double* dtgMargin = nullptr;
    unsigned int hourInYear = 0;
    unsigned int year = 0;
    Date::Calendar* calendar = nullptr;
    std::string areaName;
};

void computeMaxMRG(double* maxMrgOut, const MaxMRGinput& in);

class MaxMrgDataFactory
{
public:
    MaxMrgDataFactory(const State& state, unsigned int numSpace);
    virtual MaxMRGinput data() = 0;

protected:
    // in data
    const State& state_;
    const unsigned int numSpace_ = 0;
    RESULTATS_HORAIRES& weeklyResults_;
    // data to be built
    MaxMRGinput maxMRGinput_;
};

class MaxMrgUsualDataFactory: public MaxMrgDataFactory
{
    using MaxMrgDataFactory::MaxMrgDataFactory;

public:
    virtual MaxMRGinput data() override;
};

class MaxMrgCSRdataFactory: public MaxMrgDataFactory
{
    using MaxMrgDataFactory::MaxMrgDataFactory;

public:
    virtual MaxMRGinput data() override;
};

} // namespace Antares::Solver::Variable::Economy
