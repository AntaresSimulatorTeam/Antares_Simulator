#include "adq_patch_local_matching.h"


namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo, int Interco)
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

ntcSetToZeroStatus_AdqPatchStep1 SetNTCForAdequacyFirstStepOriginNodeInsideAdq(
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

ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return setToZeroNTCfromOutToIn_AdqPatch ? setToZero : setExtremityOrigineToZero;
    case physicalAreaOutsideAdqPatch:
        return setToZeroNTCfromOutToOut_AdqPatch ? setToZero : leaveLocalValues;
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
        case setOrigineExtremityToZero:
        {
            Xmax = 0.;
            Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
            break;
        }
        case setExtremityOrigineToZero:
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

