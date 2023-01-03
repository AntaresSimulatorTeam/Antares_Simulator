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
#include <cassert>

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
static LinkCapacityForAdequacyPatchFirstStep getNTC_OriginNodeInside(
  AdequacyPatchMode extremityType)
{
    switch (extremityType)
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
 * @param extremityType uint: The adq type of the node at the end of the link.
 *
 * @param SetToZero12Links bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param SetToZero11Links bool: Switch to cut links between nodes outside adq patch
 * (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static LinkCapacityForAdequacyPatchFirstStep getNTC_OriginNodeOutside(
  AdequacyPatchMode extremityType,
  bool SetToZero12Links,
  bool SetToZero11Links)
{
    switch (extremityType)
    {
    case physicalAreaInsideAdqPatch:
        return SetToZero12Links ? setToZero : setExtremityOriginToZero;
    case physicalAreaOutsideAdqPatch:
        return SetToZero11Links ? setToZero : leaveLocalValues;
    default:
        return leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch.
 *
 * @param originType uint: The adq type of the node at the start of the link.
 *
 * @param extremityType uint: The adq type of the node at the end of the link.
 *
 * @param SetToZero12Links bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param SetToZero11Links bool: Switch to cut links between nodes outside adq patch
 * (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static LinkCapacityForAdequacyPatchFirstStep getNTCStatus(const PROBLEME_HEBDO* ProblemeHebdo,
                                                          const int Interco)
{
    assert(ProblemeHebdo);
    assert(ProblemeHebdo);

    const AdequacyPatchMode originType
      = ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco];
    const AdequacyPatchMode extremityType
      = ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco];
    const bool SetToZero12Links = ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;
    const bool SetToZero11Links = ProblemeHebdo->adqPatchParams->SetNTCOutsideToOutsideToZero;

    switch (originType)
    {
    case physicalAreaInsideAdqPatch:
        return getNTC_OriginNodeInside(extremityType);

    case physicalAreaOutsideAdqPatch:
        return getNTC_OriginNodeOutside(extremityType, SetToZero12Links, SetToZero11Links);
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
    const auto ntcToZeroStatus = getNTCStatus(ProblemeHebdo, Interco);
    switch (ntcToZeroStatus)
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
    case setExtremityOriginToZero:
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
