/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__
#define __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
inline Content::Content() : hasYearByYear(false), hasConcatenedYbY(false)
{
    ybyInterval[0] = (uint)-1;
    ybyInterval[1] = 0;
}

inline Content::~Content()
{
}

inline bool Content::SimulationType::empty() const
{
    return areas.empty() && links.empty();
}

inline bool Content::empty() const
{
    return economy.empty() && adequacy.empty();
}

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__
