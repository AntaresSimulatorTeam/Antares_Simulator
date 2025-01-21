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
#pragma once

#include <string>
#include <vector>

#include <yuni/core/string.h>

#include <antares/inifile/inifile.h>
#include <antares/study/fwd.h>

namespace Antares::Data::AdequacyPatch
{
//! A default threshold value for initiate curtailment sharing rule
const double defaultThresholdToRunCurtailmentSharing = 0.0;
//! A default threshold value for display local matching rule violations
const double defaultThresholdDisplayLocalMatchingRuleViolations = 0.0;
//! CSR variables relaxation threshold
const int defaultValueThresholdVarBoundsRelaxation = 3;

/*!
** \brief Types of Adequacy patch mode
*/
enum AdequacyPatchMode
{
    //! Virtual area in adq patch
    virtualArea = 0,
    //! Physical Area outside the adq-patch
    physicalAreaOutsideAdqPatch = 1,
    //! Physical Area inside the adq-patch
    physicalAreaInsideAdqPatch = 2
}; // enum AdequacyPatchMode

/*!
** \brief Types of Price Taking Order (PTO) for Adequacy Patch
*/
enum class AdqPatchPTO
{
    //! PTO is DENS
    isDens = 0,
    //! PTO is Load
    isLoad

}; // enum AdqPatchPTO

class CurtailmentSharing
{
public:
    //! PTO (Price Taking Order) for adequacy patch. User can choose between DENS and Load.
    AdqPatchPTO priceTakingOrder;
    //! Threshold to initiate curtailment sharing rule
    double thresholdRun;
    //! Threshold to display Local Matching Rule violations
    double thresholdDisplayViolations;
    //! CSR Variables relaxation threshold
    int thresholdVarBoundsRelaxation;
    //! Include hurdle cost in CSR cost function
    bool includeHurdleCost;
    //! Check CSR cost function prior & after CSR optimization
    bool checkCsrCostFunction;

    bool updateFromKeyValue(const Yuni::String& key, const Yuni::String& value);
    void addProperties(IniFile::Section* section) const;

    void reset();

private:
    void resetThresholds();
};

struct AdqPatchParams
{
    bool enabled;
    //! Transmission capacities from physical areas outside adequacy patch (area type 1) to
    //! physical areas inside adequacy patch (area type 2). NTC is set to null (if true)
    //! only in the first step of adequacy patch local matching rule.
    bool setToZeroOutsideInsideLinks = true;
    CurtailmentSharing curtailmentSharing;

    void reset();
    void addExcludedVariables(std::vector<std::string>&) const;
    bool updateFromKeyValue(const Yuni::String& key, const Yuni::String& value);
    void saveToINI(IniFile& ini) const;
    bool checkAdqPatchParams(const SimulationMode simulationMode,
                             const AreaList& areas,
                             const bool includeHurdleCostParameters) const;

    void checkAdqPatchSimulationModeEconomyOnly(const SimulationMode simulationMode) const;
    void checkAdqPatchContainsAdqPatchArea(const Antares::Data::AreaList& areas) const;
    void checkAdqPatchIncludeHurdleCost(const bool includeHurdleCost) const;
};

} // namespace Antares::Data::AdequacyPatch
