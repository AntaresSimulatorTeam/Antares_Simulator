#include "adq_patch_local_matching.h"

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is inside adq path (type 2).
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 SetNTCForAdequacyFirstStepOriginNodeInsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
    case physicalAreaOutsideAdqPatch:
        return setToZero;
    default:
        return leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is outside adq path (type 1).
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @param setToZeroNTCfromOutToIn_AdqPatch bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param setToZeroNTCfromOutToOut_AdqPatch bool: Switch to cut links between nodes outside adq
 * patch (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return setToZeroNTCfromOutToIn_AdqPatch ? setToZero : setExtremityOriginToZero;
    case physicalAreaOutsideAdqPatch:
        return setToZeroNTCfromOutToOut_AdqPatch ? setToZero : leaveLocalValues;
    default:
        return leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch.
 *
 * @param ProblemeHebdo PROBLEME_HEBDO*: Weekly problem structure.
 *
 * @param Interco int: Index of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo,
                                                           int Interco)
{
    AdequacyPatchMode OriginNodeAdequacyPatchType
      = ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco];
    AdequacyPatchMode ExtremityNodeAdequacyPatchType
      = ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco];
    bool setToZeroNTCfromOutToIn_AdqPatch
      = ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;
    bool setToZeroNTCfromOutToOut_AdqPatch
      = ProblemeHebdo->adqPatchParams->SetNTCOutsideToOutsideToZero;

    switch (OriginNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return SetNTCForAdequacyFirstStepOriginNodeInsideAdq(ExtremityNodeAdequacyPatchType);

    case physicalAreaOutsideAdqPatch:
        return getNTCtoZeroStatusOriginNodeOutsideAdq(ExtremityNodeAdequacyPatchType,
                                                      setToZeroNTCfromOutToIn_AdqPatch,
                                                      setToZeroNTCfromOutToOut_AdqPatch);
    default:
        return leaveLocalValues;
    }
}

void setNTCbounds(double& Xmax,
                  double& Xmin,
                  VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo)
{
    ntcSetToZeroStatus_AdqPatchStep1 ntcToZeroStatusForAdqPatch;

    // set as default values
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

    // set for adq patch first step
    if (ProblemeHebdo->adqPatchParams && ProblemeHebdo->adqPatchParams->AdequacyFirstStep)
    {
        ntcToZeroStatusForAdqPatch = getNTCtoZeroStatus(ProblemeHebdo, Interco);

        switch (ntcToZeroStatusForAdqPatch)
        {
        case setToZero:
        {
            Xmax = 0.;
            Xmin = 0.;
            break;
        }
        case setOriginExtremityToZero:
        {
            Xmax = 0.;
            Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
            break;
        }
        case setExtremityOriginToZero:
        {
            Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
            Xmin = 0.;
            break;
        }
        }
    }
}

} // namespace AdequacyPatch
} // end namespace Data
} // namespace Antares
