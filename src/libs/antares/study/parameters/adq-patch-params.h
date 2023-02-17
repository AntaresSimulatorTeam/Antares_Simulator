#pragma once

#include <vector>
#include <string>

#include <yuni/core/string.h>
#include "antares/inifile/inifile.h"

using namespace Yuni;

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
** \brief Setting Link Capacity (NTC) for Adequacy patch first step
*/
enum class NtcSetToZeroStatus_AdqPatchStep1
{
    //! Leave NTC local values
    leaveLocalValues = 0,
    //! Set NTC to zero
    setToZero,
    //! set only origine->extremity NTC to zero
    setOriginExtremityToZero,
    //! set only extremity->origine NTC to zero
    setExtremityOriginToZero

}; // enum NTC

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


struct LocalMatching
{
    //! Transmission capacities from physical areas outside adequacy patch (area type 1) to
    //! physical areas inside adequacy patch (area type 2). NTC is set to null (if true)
    //! only in the first step of adequacy patch local matching rule.
    bool setToZeroOutsideInsideLinks = true;
    //! Transmission capacities between physical areas outside adequacy patch (area type 1).
    //! NTC is set to null (if true) only in the first step of adequacy patch local matching
    //! rule.
    bool setToZeroOutsideOutsideLinks = true;
    /*!
        ** \brief Reset to default values related to local matching
        */
    void reset();
    bool updateFromKeyValue(const String& key, const String& value);
    void addProperties(IniFile::Section* section) const;

};

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

    bool updateFromKeyValue(const String& key, const String& value);
    void addProperties(IniFile::Section* section) const;
    
    void reset();

private:
    void resetThresholds();
};


struct AdqPatchParams
{
    
    bool enabled;
    LocalMatching localMatching;
    CurtailmentSharing curtailmentSharing;

    void reset();
    void addExcludedVariables(std::vector<std::string>&) const;
    bool updateFromKeyValue(const String& key, const String& value);
    void saveToINI(IniFile& ini) const;
};

} // Antares::Data