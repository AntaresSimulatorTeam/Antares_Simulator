// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

#include <yuni/core/string.h>

#include <antares/inifile/inifile.h>
#include <antares/study/fwd.h>
#include "antares/exception/LoadingError.hpp"

namespace Antares::Error
{
class NoAreaInsideAdqPatchMode final: public LoadingError
{
public:
    NoAreaInsideAdqPatchMode();
};

class IncompatibleHurdleCostCSR final: public LoadingError
{
public:
    IncompatibleHurdleCostCSR();
};

class IncompatibleSimulationModeForAdqPatch final: public LoadingError
{
public:
    IncompatibleSimulationModeForAdqPatch();
};

} // namespace Antares::Error

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

class CurtailmentSharing final
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

    // TODO VP: remove this comment
    // This parameters does nothing right now, it will be implemented later in 9.3.x
    bool redispatch = false;

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
