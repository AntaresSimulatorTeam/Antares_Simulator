// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <map>
#include <stdexcept>

#include "antares/optimisation/linear-problem-api/IScenario.h"

namespace Antares::Optimisation::LinearProblemDataImpl
{
class Scenario final: public LinearProblemApi::IScenario
{
public:
    using IScenario::IScenario;

    [[nodiscard]] TimeSeriesNumber getData(Year year) const override;

    void setTimeSerieNumber(Year year, TimeSeriesNumber timeSeriesNumber);

    class AlreadyExists final: public std::invalid_argument
    {
    public:
        explicit AlreadyExists(const std::string& groupId);
    };

    class ScenarioNotExist final: public std::invalid_argument
    {
    public:
        explicit ScenarioNotExist(const std::string& groupId, Year year);
    };

private:
    std::map<Year, TimeSeriesNumber> timeSerieData_;
};
} // namespace Antares::Optimisation::LinearProblemDataImpl
