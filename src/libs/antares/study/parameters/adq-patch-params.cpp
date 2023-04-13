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

bool LocalMatching::updateFromKeyValue(const String& key, const String& value)
{
    if (key == "set-to-null-ntc-from-physical-out-to-physical-in-for-first-step")
        return value.to<bool>(setToZeroOutsideInsideLinks);
    if (key == "set-to-null-ntc-between-physical-out-for-first-step")
        return value.to<bool>(setToZeroOutsideOutsideLinks);
    return false;
}

void LocalMatching::addProperties(IniFile::Section* section) const
{
    section->add("set-to-null-ntc-from-physical-out-to-physical-in-for-first-step", setToZeroOutsideInsideLinks);
    section->add("set-to-null-ntc-between-physical-out-for-first-step", setToZeroOutsideOutsideLinks);
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

bool CurtailmentSharing::updateFromKeyValue(const String& key, const String& value)
{
    // Price taking order
    if (key == "price-taking-order")
        return StringToPriceTakingOrder(value, priceTakingOrder);
    // Include Hurdle Cost
    if (key == "include-hurdle-cost-csr")
        return value.to<bool>(includeHurdleCost);
    // Check CSR cost function prior and after CSR
    if (key == "check-csr-cost-function")
        return value.to<bool>(checkCsrCostFunction);
    // Thresholds
    if (key == "threshold-initiate-curtailment-sharing-rule")
        return value.to<double>(thresholdRun);
    if (key == "threshold-display-local-matching-rule-violations")
        return value.to<double>(thresholdDisplayViolations);
    if (key == "threshold-csr-variable-bounds-relaxation")
        return value.to<int>(thresholdVarBoundsRelaxation);

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
    // TODO[FOM] check this
    // If the adequacy patch is enabled, but the LMR is disabled, the DENS variable shouldn't exist
    if (enabled && !localMatching.enabled)
    {
        out.emplace_back("DENS");
    }
}


bool AdqPatchParams::updateFromKeyValue(const String& key, const String& value)
{
    if (key == "include-adq-patch")
        return value.to<bool>(enabled);

    return curtailmentSharing.updateFromKeyValue(key, value) != localMatching.updateFromKeyValue(key, value); // XOR
}

void AdqPatchParams::saveToINI(IniFile& ini) const
{
    auto* section = ini.addSection("adequacy patch");
    section->add("include-adq-patch", enabled);

    localMatching.addProperties(section);
    curtailmentSharing.addProperties(section);
}


} // Antares::Data::AdequacyPatch