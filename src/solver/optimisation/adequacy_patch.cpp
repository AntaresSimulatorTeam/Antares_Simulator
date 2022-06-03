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

namespace Antares::Data::AdequacyPatch
{
LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStep(
  AdequacyPatchMode OriginNodeAdequacyPatchType,
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool SetToZero12LinksForAdequacyPatch,
  bool SetToZero11LinksForAdequacyPatch)
{
    switch (OriginNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return SetNTCForAdequacyFirstStepOriginNodeInsideAdq(ExtremityNodeAdequacyPatchType,
                                                             SetToZero12LinksForAdequacyPatch);

    case physicalAreaOutsideAdqPatch:
        return SetNTCForAdequacyFirstStepOriginNodeOutsideAdq(ExtremityNodeAdequacyPatchType,
                                                              SetToZero12LinksForAdequacyPatch,
                                                              SetToZero11LinksForAdequacyPatch);
    default:
        return leaveLocalValues;
    }
}

LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStepOriginNodeInsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool SetToZero12LinksForAdequacyPatch)
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

LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStepOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool SetToZero12LinksForAdequacyPatch,
  bool SetToZero11LinksForAdequacyPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return SetToZero12LinksForAdequacyPatch ? setToZero : setExtremityOrigineToZero;
    case physicalAreaOutsideAdqPatch:
        return SetToZero11LinksForAdequacyPatch ? setToZero : leaveLocalValues;
    default:
        return leaveLocalValues;
    }
}

void setBoundsAdqPatch(double& Xmax,
                       double& Xmin,
                       VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                       const int Interco,
                       PROBLEME_HEBDO* ProblemeHebdo)
{
    LinkCapacityForAdequacyPatchFirstStep SetToZeroLinkNTCForAdequacyPatchFirstStep;

    SetToZeroLinkNTCForAdequacyPatchFirstStep = SetNTCForAdequacyFirstStep(
      ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco],
      ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco],
      ProblemeHebdo->adqPatch->SetNTCOutsideToInsideToZero,
      ProblemeHebdo->adqPatch->SetNTCOutsideToOutsideToZero);

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
    default:
    {
        setBoundsNoAdqPatch(Xmax, Xmin, ValeursDeNTC, Interco);
        break;
    }
    }
}

void setBoundsNoAdqPatch(double& Xmax,
                         double& Xmin,
                         VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                         const int Interco)
{
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
}
} // namespace Antares::Data::AdequacyPatch
