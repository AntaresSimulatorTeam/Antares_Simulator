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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Default
{
//! Border color
inline wxColour BorderColor()
{
    return wxColour(235, 235, 235);
}

//! Border hightlight color
inline wxColour BorderHighlightColor()
{
    return wxColour(140, 140, 140);
}

//! Border hightlight color
inline wxColour BorderDaySeparator()
{
    return wxColour(170, 150, 150);
}

//! Border hightlight color
inline wxColour BorderMonthSeparator()
{
    return wxColour(110, 110, 220);
}

} // namespace Default
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__
