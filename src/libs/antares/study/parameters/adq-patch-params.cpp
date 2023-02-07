#include "adq-patch-params.h"
#include "antares/logs.h"

namespace Antares::Data::AdequacyPatch
{

// -------------------
// Local matching
// -------------------

void LocalMatching::reset()
{
    setToZeroOutsideInsideLinks = true;
    setToZeroOutsideOutsideLinks = true;
}


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


// ------------------------
// Adq patch parameters
// ------------------------
void AdqPatchParams::reset()
{
    enabled = false;

    localMatching.reset();
    curtailmentSharing.reset();
}

void AdqPatchParams::addExcludedVariables(std::vector<std::string>& out) const
{
    if (!enabled)
    {
        out.emplace_back("DENS");
        out.emplace_back("LMR VIOL.");
        out.emplace_back("SPIL. ENRG. CSR");
        out.emplace_back("DTG MRG CSR");
    }
}

static bool StringToPriceTakingOrder(const AnyString& PTO_as_string, AdequacyPatch::AdqPatchPTO& PTO_as_enum)
{
    CString<24, false> s = PTO_as_string;
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


bool AdqPatchParams::updateFromKeyValue(const String& key, const String& value)
{
    if (key == "include-adq-patch")
        return value.to<bool>(enabled);
    if (key == "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step")
        return value.to<bool>(localMatching.setToZeroOutsideInsideLinks);
    if (key == "set-to-null-ntc-between-physical-out-for-first-step")
        return value.to<bool>(localMatching.setToZeroOutsideOutsideLinks);
    // Price taking order
    if (key == "price-taking-order")
        return StringToPriceTakingOrder(value, curtailmentSharing.priceTakingOrder);
    // Include Hurdle Cost
    if (key == "include-hurdle-cost-csr")
        return value.to<bool>(curtailmentSharing.includeHurdleCost);
    // Check CSR cost function prior and after CSR
    if (key == "check-csr-cost-function")
        return value.to<bool>(curtailmentSharing.checkCsrCostFunction);
    // Thresholds
    if (key == "threshold-initiate-curtailment-sharing-rule")
        return value.to<double>(curtailmentSharing.thresholdRun);
    if (key == "threshold-display-local-matching-rule-violations")
        return value.to<double>(curtailmentSharing.thresholdDisplayViolations);
    if (key == "threshold-csr-variable-bounds-relaxation")
        return value.to<int>(curtailmentSharing.thresholdVarBoundsRelaxation);

    return false;
}

void AdqPatchParams::saveToINI(IniFile& ini) const
{
    auto* section = ini.addSection("adequacy patch");
    section->add("include-adq-patch", enabled);
    section->add("set-to-null-ntc-from-physical-out-to-physical-in-for-first-step",
        localMatching.setToZeroOutsideInsideLinks);
    section->add("set-to-null-ntc-between-physical-out-for-first-step",
        localMatching.setToZeroOutsideOutsideLinks);
    section->add("price-taking-order",
        PriceTakingOrderToString(curtailmentSharing.priceTakingOrder));
    section->add("include-hurdle-cost-csr", curtailmentSharing.includeHurdleCost);
    section->add("check-csr-cost-function", curtailmentSharing.checkCsrCostFunction);
    // Thresholds
    section->add("threshold-initiate-curtailment-sharing-rule",
        curtailmentSharing.thresholdRun);
    section->add("threshold-display-local-matching-rule-violations",
        curtailmentSharing.thresholdDisplayViolations);
    section->add("threshold-csr-variable-bounds-relaxation",
        curtailmentSharing.thresholdVarBoundsRelaxation);
}


} // Antares::Data::AdequacyPatch