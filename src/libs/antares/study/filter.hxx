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
#ifndef __ANTARES_LIB_STUDY_FILTER_HXX__
#define __ANTARES_LIB_STUDY_FILTER_HXX__

namespace Antares
{
namespace Data
{
template<class StringT>
void AppendFilterToString(StringT& out, uint filter)
{
    if (filter)
    {
        bool empty = true;
        if (0 != (filter & filterHourly))
        {
            out << "hourly";
            empty = false;
        }
        if (0 != (filter & filterDaily))
        {
            if (!empty)
                out << ", ";
            out << "daily";
            empty = false;
        }
        if (0 != (filter & filterWeekly))
        {
            if (!empty)
                out << ", ";
            out << "weekly";
            empty = false;
        }
        if (0 != (filter & filterMonthly))
        {
            if (!empty)
                out << ", ";
            out << "monthly";
            empty = false;
        }
        if (0 != (filter & filterAnnual))
        {
            if (!empty)
                out << ", ";
            out << "annual";
        }
    }
}

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIB_STUDY_FILTER_HXX__
