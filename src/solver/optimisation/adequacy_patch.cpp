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

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
AdequacyPatch::LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStep(
  bool AdequacyFirstStep,
  AdequacyPatch::AdequacyPatchMode OriginNodeAdequacyPatchType,
  AdequacyPatch::AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  std::map<std::pair<AdequacyPatchMode, AdequacyPatchMode>, LinkCapacityForAdequacyPatchFirstStep>
    &behaviorMap)
{
    if (AdequacyFirstStep)
    {
        auto search = behaviorMap.find(
          std::make_pair(OriginNodeAdequacyPatchType, ExtremityNodeAdequacyPatchType));
        if (search != behaviorMap.end())
            return search->second;
        else
            return leaveLocalValues;
    }
    else
        return leaveLocalValues;
}

std::map<std::pair<AdequacyPatchMode, AdequacyPatchMode>, LinkCapacityForAdequacyPatchFirstStep>
  GenerateLinkRestrictionMapForAdqFirstStep(bool SetToZero12LinksForAdequacyPatch,
                                            bool SetToZero11LinksForAdequacyPatch)
{
    std::map<std::pair<AdequacyPatchMode, AdequacyPatchMode>, LinkCapacityForAdequacyPatchFirstStep>
      behaviorMap;

    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaInsideAdqPatch, adqmPhysicalAreaInsideAdqPatch), setToZero));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaOutsideAdqPatch, adqmPhysicalAreaInsideAdqPatch),
      (SetToZero12LinksForAdequacyPatch) ? setToZero : setExtremityOrigineToZero));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaInsideAdqPatch, adqmPhysicalAreaOutsideAdqPatch),
      (SetToZero12LinksForAdequacyPatch) ? setToZero : setOrigineExtremityToZero));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaOutsideAdqPatch, adqmPhysicalAreaOutsideAdqPatch),
      (SetToZero11LinksForAdequacyPatch) ? setToZero : leaveLocalValues));
    behaviorMap.insert(
      std::make_pair(std::make_pair(adqmVirtualArea, adqmVirtualArea), leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmVirtualArea, adqmPhysicalAreaOutsideAdqPatch), leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaOutsideAdqPatch, adqmVirtualArea), leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmVirtualArea, adqmPhysicalAreaInsideAdqPatch), leaveLocalValues));
    behaviorMap.insert(std::make_pair(
      std::make_pair(adqmPhysicalAreaInsideAdqPatch, adqmVirtualArea), leaveLocalValues));

    return behaviorMap;
}

} // namespace AdequacyPatch
} // namespace Data
} // namespace Antares
