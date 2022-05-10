/*
** Copyright 2007-2018 RTE
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

#include "../simulation/simulation.h"
#include "adequacy_patch.h"

using namespace Antares::Data::AdequacyPatch;

LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStep(
  AdequacyPatchMode OriginNodeAdequacyPatchType,
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    LinkCapacityForAdequacyPatchFirstStep returnNTC;

    switch (OriginNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        returnNTC = SetNTCForAdequacyFirstStepOriginNodeInsideAdq(ExtremityNodeAdequacyPatchType,
                                                                  setToZeroNTCfromOutToIn_AdqPatch);
        break;
    case physicalAreaOutsideAdqPatch:
        returnNTC
          = SetNTCForAdequacyFirstStepOriginNodeOutsideAdq(ExtremityNodeAdequacyPatchType,
                                                           setToZeroNTCfromOutToIn_AdqPatch,
                                                           setToZeroNTCfromOutToOut_AdqPatch);
        break;
    default:
        returnNTC = leaveLocalValues;
        break;
    }
    return returnNTC;
}

LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStepOriginNodeInsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch)
{
    LinkCapacityForAdequacyPatchFirstStep returnNTC;

    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        returnNTC = setToZero;
        break;
    case physicalAreaOutsideAdqPatch:
        returnNTC = (setToZeroNTCfromOutToIn_AdqPatch) ? setToZero : setOrigineExtremityToZero;
        break;
    default:
        returnNTC = leaveLocalValues;
        break;
    }
    return returnNTC;
}

LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStepOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    LinkCapacityForAdequacyPatchFirstStep returnNTC;

    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        returnNTC = (setToZeroNTCfromOutToIn_AdqPatch) ? setToZero : setExtremityOrigineToZero;
        break;
    case physicalAreaOutsideAdqPatch:
        returnNTC = (setToZeroNTCfromOutToOut_AdqPatch) ? setToZero : leaveLocalValues;
        break;
    default:
        returnNTC = leaveLocalValues;
        break;
    }
    return returnNTC;
}

void setNTCbounds(double& Xmax,
                  double& Xmin,
                  VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo)
{
    LinkCapacityForAdequacyPatchFirstStep SetToZeroLinkNTCForAdequacyPatchFirstStep;

    // set as default values
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

    // set for adq patch first step
    if (ProblemeHebdo->adqPatch && ProblemeHebdo->adqPatch->AdequacyFirstStep)
    {
        SetToZeroLinkNTCForAdequacyPatchFirstStep = SetNTCForAdequacyFirstStep(
          ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco],
          ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco],
          ProblemeHebdo->adqPatch->LinkCapacityForAdqPatchFirstStepFromAreaOutsideToAreaInsideAdq,
          ProblemeHebdo->adqPatch->LinkCapacityForAdqPatchFirstStepBetweenAreaOutsideAdq);

        switch (SetToZeroLinkNTCForAdequacyPatchFirstStep)
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
