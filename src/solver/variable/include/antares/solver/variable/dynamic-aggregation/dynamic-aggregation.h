/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/storage/averagedata.h"
#include "antares/solver/variable/storage/minmax-data.h"

namespace Antares::Solver::Variable
{

class SetData
{
public:
    SetData(const std::set<Data::Area*, Data::CompareAreaName>& set, Data::Study& study);

    void addResultsToSet(const PROBLEME_HEBDO& pb);
    void mergeAnother(const SetData& toMerge, double ratio, Data::Study& study);

    // TODO rm tests
    void writeResultsToFolder(const std::string& folderName) const;

protected:
    std::vector<std::vector<long double>> thermalResults_;
    std::vector<std::vector<long double>> renewableResults_;

    std::vector<std::vector<long double>> stsInjectionResults_;
    std::vector<std::vector<long double>> stsWithdrawalResults_;
    std::vector<std::vector<long double>> stsLevelResults_;

    std::set<std::string> thermalGroupNames_;
    std::map<std::string, unsigned int> thermalGroupToNumbers_;

    std::set<std::string> renewableGroupNames_;
    std::map<std::string, unsigned int> renewableGroupToNumbers_;

    std::set<std::string> stsGroupNames_;
    std::map<std::string, unsigned int> stsGroupToNumbers_;
    const std::set<Data::Area*, Data::CompareAreaName>& set_;

    R::AllYears::MinMaxData minThermal;
    R::AllYears::MinMaxData maxThermal;

    R::AllYears::MinMaxData minRenewable;
    R::AllYears::MinMaxData maxRenewable;

    R::AllYears::MinMaxData minStsInjection;
    R::AllYears::MinMaxData maxStsInjection;

    R::AllYears::MinMaxData minStsWithdrawal;
    R::AllYears::MinMaxData maxStsWithdrawal;

    R::AllYears::MinMaxData minStsLevel;
    R::AllYears::MinMaxData maxStsLevel;

    R::AllYears::AverageData averageThermal;
    R::AllYears::AverageData averageRenewable;
    R::AllYears::AverageData averageStsInjection;
    R::AllYears::AverageData averageStsWithdrawal;
    R::AllYears::AverageData averageStsLevel;
};

class DynamicAggregation
{
public:
    explicit DynamicAggregation(Data::Study& study);

    void addResultsToSets(const PROBLEME_HEBDO& pb);
    void mergeAnother(const DynamicAggregation& toMerge, double ratio);

    // TODO rm tests
    void writeAllResults(const std::string& baseFolder) const;

private:
    Data::Study& study_;
    std::map<std::string, SetData> setsData_;
};

} // namespace Antares::Solver::Variable
