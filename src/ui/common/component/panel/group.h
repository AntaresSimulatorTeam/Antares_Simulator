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
#ifndef __ANTARES_COMMON_COMPONENTS_PANEL_GROUP_H__
#define __ANTARES_COMMON_COMPONENTS_PANEL_GROUP_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include "panel.h"

namespace Antares
{
namespace Component
{
/*!
** \brief Panel implementation
*/
class PanelGroup : public Panel
{
public:
    /*!
    ** \brief Set the background color of a component used in a PanelGroup
    */
    static void SetDarkBackgroundColor(wxWindow* ctrl, int lightModifier = 0);

    /*!
    ** \brief Set the background color of a component used in a PanelGroup
    */
    static void SetLighterBackgroundColor(wxWindow* ctrl, int lightModifier = 0);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    PanelGroup(wxWindow* parent, const char* image = NULL);
    //! Destructor
    virtual ~PanelGroup();
    //@}

public:
    //! Subpanel
    Panel* subpanel;
    //! The leftSizer
    wxSizer* leftSizer;

}; // class PanelGroup

} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
