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

using namespace Antares;
using namespace Antares::Data;
using namespace Antares::Data::AdequacyPatch;

uint SetNTCForAdequacyFirstStep(bool AdequacyFirstStep,
                                uint StartNodeAdequacyPatchType,
                                uint EndNodeAdequacyPatchType,
                                bool SetToZero12LinksForAdequacyPatch,
                                bool SetToZero11LinksForAdequacyPatch)
{
    if (AdequacyFirstStep)
    {
        if (StartNodeAdequacyPatchType == adqmPhysicalAreaInsideAdqPatch
            && EndNodeAdequacyPatchType == adqmPhysicalAreaInsideAdqPatch)
            return setToZero;
        else if (StartNodeAdequacyPatchType == adqmPhysicalAreaOutsideAdqPatch
                 && EndNodeAdequacyPatchType == adqmPhysicalAreaInsideAdqPatch)
        {
            if (SetToZero12LinksForAdequacyPatch)
                return setToZero;
            else
                return setExtremityOrigineToZero;
        }
        else if (StartNodeAdequacyPatchType == adqmPhysicalAreaInsideAdqPatch
                 && EndNodeAdequacyPatchType == adqmPhysicalAreaOutsideAdqPatch)
        {
            if (SetToZero12LinksForAdequacyPatch)
                return setToZero;
            else
                return setOrigineExtremityToZero;
        }
        else if (StartNodeAdequacyPatchType == adqmPhysicalAreaOutsideAdqPatch
                 && EndNodeAdequacyPatchType == adqmPhysicalAreaOutsideAdqPatch)
        {
            if (SetToZero11LinksForAdequacyPatch)
                return setToZero;
            else
                return leaveLocalValues;
        }
        else
            return leaveLocalValues;
    }
    else
        return leaveLocalValues;
}
