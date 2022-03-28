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
#include <map>

using namespace Antares::Data;

AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStep(
  AdequacyPatch::AdequacyPatchMode OriginNodeAdequacyPatchType,
  AdequacyPatch::AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  std::map<adqPair, AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep>& behaviorMap)
{
    auto search = behaviorMap.find(
      std::make_pair(OriginNodeAdequacyPatchType, ExtremityNodeAdequacyPatchType));
    if (search != behaviorMap.end())
        return search->second;
    else
        return leaveLocalValues;
}

std::map<adqPair, AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep> GenerateLinkRestrictionMapForAdqFirstStep(
  bool SetToZero12LinksForAdequacyPatch,
  bool SetToZero11LinksForAdequacyPatch)
{
    std::map<adqPair, AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep> behaviorMap;
    // No transfer of energy possible between physical areas part of the adequacy patch
    behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaInsideAdqPatch, AdequacyPatch::adqmPhysicalAreaInsideAdqPatch), AdequacyPatch::setToZero));
    // all else remains the same
    behaviorMap.insert(
      std::make_pair(std::make_pair(AdequacyPatch::adqmVirtualArea, AdequacyPatch::adqmVirtualArea), AdequacyPatch::leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmVirtualArea, AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch), AdequacyPatch::leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch, AdequacyPatch::adqmVirtualArea), AdequacyPatch::leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmVirtualArea, AdequacyPatch::adqmPhysicalAreaInsideAdqPatch), AdequacyPatch::leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaInsideAdqPatch, AdequacyPatch::adqmVirtualArea), AdequacyPatch::leaveLocalValues));
    // except if the options SetToZero12LinksForAdequacyPatch and/or SetToZero11LinksForAdequacyPatch are
    if (SetToZero12LinksForAdequacyPatch){
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch, AdequacyPatch::adqmPhysicalAreaInsideAdqPatch), AdequacyPatch::setToZero));
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaInsideAdqPatch, AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch), AdequacyPatch::setToZero));
    }
    else{
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch, AdequacyPatch::adqmPhysicalAreaInsideAdqPatch), AdequacyPatch::setExtremityOrigineToZero));
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaInsideAdqPatch, AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch), AdequacyPatch::setOrigineExtremityToZero));
    }

    if (SetToZero11LinksForAdequacyPatch){
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch, AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch), AdequacyPatch::setToZero));
    }
    else {
      behaviorMap.insert(std::make_pair(
      std::make_pair(AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch, AdequacyPatch::adqmPhysicalAreaOutsideAdqPatch), AdequacyPatch::leaveLocalValues));
    }

    return behaviorMap;
}

void setBoundsAdqPatch(double& Xmax,
                       double& Xmin,
                       VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                       const int Interco,
                       PROBLEME_HEBDO* ProblemeHebdo)
{
    AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep
      SetToZeroLinkNTCForAdequacyPatchFirstStep;

    SetToZeroLinkNTCForAdequacyPatchFirstStep
      = SetNTCForAdequacyFirstStep(
        ProblemeHebdo->adequacyPatchRuntimeData.originAreaType[Interco],
        ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaType[Interco],
        ProblemeHebdo->adqPatch->AdqBehaviorMap);

    if (SetToZeroLinkNTCForAdequacyPatchFirstStep == AdequacyPatch::setToZero)
    {
        Xmax = 0.;
        Xmin = 0.;
    }
    else if (SetToZeroLinkNTCForAdequacyPatchFirstStep
             == AdequacyPatch::setOrigineExtremityToZero)
    {
        Xmax = 0.;
        Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
    }
    else if (SetToZeroLinkNTCForAdequacyPatchFirstStep
             == AdequacyPatch::setExtremityOrigineToZero)
    {
        Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
        Xmin = 0.;
    }
    else
    {
        Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
        Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
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

