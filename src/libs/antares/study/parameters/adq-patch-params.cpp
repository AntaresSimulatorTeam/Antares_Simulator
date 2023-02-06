#include "adq-patch-params.h"

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


}