// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__


namespace Antares::Component::Datagrid::Default
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

} // namespace Antares::Component::Datagrid::Default

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DEFAULT_H__
