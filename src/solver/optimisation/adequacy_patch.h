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

#ifndef __SOLVER_ADEQUACY_FUNCTIONS_H__
#define __SOLVER_ADEQUACY_FUNCTIONS_H__

typedef std::pair<AdequacyPatchMode, AdequacyPatchMode> adqPair;

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{

/*!
 * Determines if a link capacity needs to be set to 0. Only changes something if used during the
 * AdequacyFirstStep.
 *
 * @param AdequacyFirstStep boolean for the first run of the optimization used by the adequacy patch
 *
 * @param OriginNodeAdequacyPatchType uint: The adq type of the node at the start of the link.
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @param behaviorMap map: map containing link capacity restrictions for every possible combination
 * of adq types for node at start and node at end of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
LinkCapacityForAdequacyPatchFirstStep SetNTCForAdequacyFirstStep(
  bool AdequacyFirstStep,
  AdequacyPatchMode OriginNodeAdequacyPatchType,
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  std::map<adqPair, LinkCapacityForAdequacyPatchFirstStep>
    &behaviorMap);

/*!
 * Generates map containing all possible relations of start&end area adq patch mode taking into
 * consideration user interface input options.
 *
 * @param SetToZero12LinksForAdequacyPatch bool: Switch to cut links from nodes of adq type 1
 * towards nodes of adq type 2
 *
 * @param SetToZero11LinksForAdequacyPatch bool: Switch to cut links from nodes of adq type 1
 * towards nodes of adq type 1
 *
 * @return map defining link capacity restrictions is constructed according to the start&end area
 * adequacy patch mode and user interface input options (first two function parameters).
 */
std::map<adqPair, LinkCapacityForAdequacyPatchFirstStep>
  GenerateLinkRestrictionMapForAdqFirstStep(bool SetToZero12LinksForAdequacyPatch,
                                            bool SetToZero11LinksForAdequacyPatch);

} // namespace AdequacyPatch
} // namespace Data
} // namespace Antares

#endif /* __SOLVER_ADEQUACY_FUNCTIONS_H__ */
