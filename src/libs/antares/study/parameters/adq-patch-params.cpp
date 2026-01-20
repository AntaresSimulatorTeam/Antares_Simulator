// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parameters/adq-patch-params.h"

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/study/study.h>

namespace Antares::Error
{
NoAreaInsideAdqPatchMode::NoAreaInsideAdqPatchMode():
    LoadingError("Minimum one area must be inside adequacy patch mode when using adequacy patch")
{
}

IncompatibleHurdleCostCSR::IncompatibleHurdleCostCSR():
    LoadingError("Incompatible options include.hurdleCost and curtailmentSharing.includeHurdleCost")
{
}

IncompatibleSimulationModeForAdqPatch::IncompatibleSimulationModeForAdqPatch():
    LoadingError("Adequacy Patch can only be used with Economy Simulation Mode")
{
}

} // namespace Antares::Error

namespace Antares::Data::AdequacyPatch
{

// -----------------------
// Curtailment sharing
// -----------------------
void CurtailmentSharing::reset()
{
    priceTakingOrder = AdqPatchPTO::isDens;
    includeHurdleCost = false;
    checkCsrCostFunction = false;
    resetThresholds();
}

void CurtailmentSharing::resetThresholds()
{
    // Initialize all thresholds values for adequacy patch
    thresholdRun = defaultThresholdToRunCurtailmentSharing;
    thresholdDisplayViolations = defaultThresholdDisplayLocalMatchingRuleViolations;
    thresholdVarBoundsRelaxation = defaultValueThresholdVarBoundsRelaxation;
}

static bool StringToPriceTakingOrder(const AnyString& PTO_as_string,
                                     AdequacyPatch::AdqPatchPTO& PTO_as_enum)
{
    Yuni::CString<24, false> s = PTO_as_string;
    s.trim();
    s.toLower();
    if (s == "dens")
    {
        PTO_as_enum = AdequacyPatch::AdqPatchPTO::isDens;
        return true;
    }
    if (s == "load")
    {
        PTO_as_enum = AdequacyPatch::AdqPatchPTO::isLoad;
        return true;
    }

    logs.warning() << "parameters: invalid price taking order. Got '" << PTO_as_string << "'";

    return false;
}

bool CurtailmentSharing::updateFromKeyValue(const Yuni::String& key, const Yuni::String& value)
{
    // Price taking order
    if (key == "price-taking-order")
    {
        return StringToPriceTakingOrder(value, priceTakingOrder);
    }
    // Include Hurdle Cost
    if (key == "include-hurdle-cost-csr")
    {
        return value.to<bool>(includeHurdleCost);
    }
    // Check CSR cost function prior and after CSR
    if (key == "check-csr-cost-function")
    {
        return value.to<bool>(checkCsrCostFunction);
    }
    // Thresholds
    if (key == "threshold-initiate-curtailment-sharing-rule")
    {
        return value.to<double>(thresholdRun);
    }
    if (key == "threshold-display-local-matching-rule-violations")
    {
        return value.to<double>(thresholdDisplayViolations);
    }
    if (key == "threshold-csr-variable-bounds-relaxation")
    {
        return value.to<int>(thresholdVarBoundsRelaxation);
    }

    return false;
}

const char* PriceTakingOrderToString(AdequacyPatch::AdqPatchPTO pto)
{
    switch (pto)
    {
    case AdequacyPatch::AdqPatchPTO::isDens:
        return "DENS";
    case AdequacyPatch::AdqPatchPTO::isLoad:
        return "Load";
    default:
        return "";
    }
}

void CurtailmentSharing::addProperties(IniFile::Section* section) const
{
    section->add("price-taking-order", PriceTakingOrderToString(priceTakingOrder));
    section->add("include-hurdle-cost-csr", includeHurdleCost);
    section->add("check-csr-cost-function", checkCsrCostFunction);

    // Thresholds
    section->add("threshold-initiate-curtailment-sharing-rule", thresholdRun);
    section->add("threshold-display-local-matching-rule-violations", thresholdDisplayViolations);
    section->add("threshold-csr-variable-bounds-relaxation", thresholdVarBoundsRelaxation);
}

// ------------------------
// Adq patch parameters
// ------------------------
void AdqPatchParams::reset()
{
    enabled = false;

    curtailmentSharing.reset();
    setToZeroOutsideInsideLinks = true;
}

void AdqPatchParams::addExcludedVariables(std::vector<std::string>& out) const
{
    if (!enabled)
    {
        out.emplace_back("DENS");
        out.emplace_back("LMR VIOL.");
        out.emplace_back("UNSP. ENRG CSR");
        out.emplace_back("DTG MRG CSR");
        out.emplace_back("LOLD CSR");
        out.emplace_back("LOLP CSR");
        out.emplace_back("MAX MRG CSR");
        out.emplace_back("OV. COST CSR");
        out.emplace_back("MRG. PRICE CSR");
    }
}

bool AdqPatchParams::updateFromKeyValue(const Yuni::String& key, const Yuni::String& value)
{
    if (key == "include-adq-patch")
    {
        return value.to<bool>(enabled);
    }
    if (key == "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step")
    {
        return value.to<bool>(setToZeroOutsideInsideLinks);
    }
    if (key == "redispatch")
    {
        return value.to<bool>(redispatch);
    }
    return curtailmentSharing.updateFromKeyValue(key, value);
}

void AdqPatchParams::saveToINI(IniFile& ini) const
{
    auto* section = ini.addSection("adequacy patch");
    section->add("include-adq-patch", enabled);
    section->add("set-to-null-ntc-from-physical-out-to-physical-in-for-first-step",
                 setToZeroOutsideInsideLinks);

    curtailmentSharing.addProperties(section);
}

bool AdqPatchParams::checkAdqPatchParams(const SimulationMode simulationMode,
                                         const AreaList& areas,
                                         const bool includeHurdleCostParameters) const
{
    checkAdqPatchSimulationModeEconomyOnly(simulationMode);
    checkAdqPatchContainsAdqPatchArea(areas);
    checkAdqPatchIncludeHurdleCost(includeHurdleCostParameters);

    return true;
}

// Adequacy Patch can only be used with Economy Study/Simulation Mode.
void AdqPatchParams::checkAdqPatchSimulationModeEconomyOnly(
  const SimulationMode simulationMode) const
{
    if (simulationMode != SimulationMode::Economy)
    {
        throw Error::IncompatibleSimulationModeForAdqPatch();
    }
}

// When Adequacy Patch is on at least one area must be inside Adequacy patch mode.
void AdqPatchParams::checkAdqPatchContainsAdqPatchArea(const Antares::Data::AreaList& areas) const
{
    const bool containsAdqArea = std::any_of(areas.cbegin(),
                                             areas.cend(),
                                             [](const std::pair<AreaName, Area*>& area) {
                                                 return area.second->adequacyPatchMode
                                                        == physicalAreaInsideAdqPatch;
                                             });

    if (!containsAdqArea)
    {
        throw Error::NoAreaInsideAdqPatchMode();
    }
}

void AdqPatchParams::checkAdqPatchIncludeHurdleCost(const bool includeHurdleCostParameters) const
{
    if (curtailmentSharing.includeHurdleCost && !includeHurdleCostParameters)
    {
        throw Error::IncompatibleHurdleCostCSR();
    }
}
} // namespace Antares::Data::AdequacyPatch
