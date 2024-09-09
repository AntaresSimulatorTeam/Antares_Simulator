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

#include "antares/solver/optimisation/adequacy_patch_local_matching/adq_patch_local_matching.h"

#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

namespace Antares::Data::AdequacyPatch
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
static NtcSetToZeroStatus_AdqPatchStep1 SetNTCForAdequacyFirstStepOriginNodeInsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
    case physicalAreaOutsideAdqPatch:
        return NtcSetToZeroStatus_AdqPatchStep1::setToZero;
    default:
        return NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
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
static NtcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return setToZeroNTCfromOutToIn_AdqPatch
                 ? NtcSetToZeroStatus_AdqPatchStep1::setToZero
                 : NtcSetToZeroStatus_AdqPatchStep1::setExtremityOriginToZero;
    case physicalAreaOutsideAdqPatch:
        return setToZeroNTCfromOutToOut_AdqPatch
                 ? NtcSetToZeroStatus_AdqPatchStep1::setToZero
                 : NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
    default:
        return NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch.
 *
 * @param problemeHebdo PROBLEME_HEBDO*: Weekly problem structure.
 *
 * @param Interco int: Index of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static NtcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* problemeHebdo,
                                                           const AdqPatchParams& adqPatchParams,
                                                           int Interco)
{
    AdequacyPatchMode OriginNodeAdequacyPatchType = problemeHebdo->adequacyPatchRuntimeData
                                                      ->originAreaMode[Interco];
    AdequacyPatchMode ExtremityNodeAdequacyPatchType = problemeHebdo->adequacyPatchRuntimeData
                                                         ->extremityAreaMode[Interco];
    bool setToZeroNTCfromOutToIn_AdqPatch = adqPatchParams.localMatching
                                              .setToZeroOutsideInsideLinks;
    bool setToZeroNTCfromOutToOut_AdqPatch = adqPatchParams.localMatching
                                               .setToZeroOutsideOutsideLinks;

    switch (OriginNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return SetNTCForAdequacyFirstStepOriginNodeInsideAdq(ExtremityNodeAdequacyPatchType);

    case physicalAreaOutsideAdqPatch:
        return getNTCtoZeroStatusOriginNodeOutsideAdq(ExtremityNodeAdequacyPatchType,
                                                      setToZeroNTCfromOutToIn_AdqPatch,
                                                      setToZeroNTCfromOutToOut_AdqPatch);
    default:
        return NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
    }
}

void setNTCbounds(double& Xmax,
                  double& Xmin,
                  const VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* problemeHebdo,
                  const AdqPatchParams& adqPatchParams)
{
    NtcSetToZeroStatus_AdqPatchStep1 ntcToZeroStatusForAdqPatch;

    // set as default values
    Xmax = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco]);

    // set for adq patch first step
    if (adqPatchParams.enabled && adqPatchParams.localMatching.enabled
        && problemeHebdo->adequacyPatchRuntimeData->AdequacyFirstStep)
    {
        ntcToZeroStatusForAdqPatch = getNTCtoZeroStatus(problemeHebdo, adqPatchParams, Interco);

        switch (ntcToZeroStatusForAdqPatch)
        {
        case NtcSetToZeroStatus_AdqPatchStep1::setToZero:
        {
            Xmax = 0.;
            Xmin = 0.;
            break;
        }
        case NtcSetToZeroStatus_AdqPatchStep1::setOriginExtremityToZero:
        {
            Xmax = 0.;
            Xmin = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[Interco]);
            break;
        }
        case NtcSetToZeroStatus_AdqPatchStep1::setExtremityOriginToZero:
        {
            Xmax = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[Interco];
            Xmin = 0.;
            break;
        }
        default:
            return;
        }
    }
}

} // namespace Antares::Data::AdequacyPatch
