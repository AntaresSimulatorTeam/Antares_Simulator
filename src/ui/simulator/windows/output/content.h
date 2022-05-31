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
#ifndef __ANTARES_WINDOWS_OUTPUT_CONTENT_H__
#define __ANTARES_WINDOWS_OUTPUT_CONTENT_H__

#include <yuni/yuni.h>
#include <set>
#include <map>
#include "fwd.h"
#include <antares/study/fwd.h>

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
class Content final
{
public:
    using AreaNameSet = std::set<Data::AreaName>;
    using LinkNameSet = std::set<Data::AreaLinkName>;

    class SimulationType
    {
    public:
        //! Get if the result set is empty
        bool empty() const;

    public:
        //! All areas
        AreaNameSet areas;
        //! All links
        LinkNameSet links;

    }; // class SimulationType

public:
    //! Constructor & Destructor
    //@{
    //! Default contructor
    Content();
    //! Destructor
    ~Content();
    //@}

    bool empty() const;

public:
    //! All areas/links found for economy
    SimulationType economy;
    //! All areas/links found for adequacy
    SimulationType adequacy;

    //! All available clusters
    ThermalNameSetPerArea clusters;

    //! Flag to know if some year-by-year data are presetn
    bool hasYearByYear;
    //! Flag to know if concatened individual years are present
    bool hasConcatenedYbY;
    //! Year interval for year-by-year data
    uint ybyInterval[2];

}; // class Content

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#include "content.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_CONTENT_H__
