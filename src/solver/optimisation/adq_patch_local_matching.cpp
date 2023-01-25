/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "adq_patch_local_matching.h"

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
        return setToZeroNTCfromOutToIn_AdqPatch ? NtcSetToZeroStatus_AdqPatchStep1::setToZero
            : NtcSetToZeroStatus_AdqPatchStep1::setExtremityOriginToZero;
    case physicalAreaOutsideAdqPatch:
        return setToZeroNTCfromOutToOut_AdqPatch ? NtcSetToZeroStatus_AdqPatchStep1::setToZero
            : NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
    default:
        return NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
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
static NtcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo,
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
        return NtcSetToZeroStatus_AdqPatchStep1::leaveLocalValues;
    }
}

void setNTCbounds(double& Xmax,
                  double& Xmin,
                  const VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo)
{
    NtcSetToZeroStatus_AdqPatchStep1 ntcToZeroStatusForAdqPatch;

    // set as default values
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

    // set for adq patch first step
    if (ProblemeHebdo->adqPatchParams && ProblemeHebdo->adqPatchParams->AdequacyFirstStep)
    {
        ntcToZeroStatusForAdqPatch = getNTCtoZeroStatus(ProblemeHebdo, Interco);

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
            Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
            break;
        }
        case NtcSetToZeroStatus_AdqPatchStep1::setExtremityOriginToZero:
        {
            Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
            Xmin = 0.;
            break;
        }
        default:
            return;
        }
    }
}

} // namespace Antares::Data::AdequacyPatch
