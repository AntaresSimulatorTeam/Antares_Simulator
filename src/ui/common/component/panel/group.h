// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENTS_PANEL_GROUP_H__
#define __ANTARES_COMMON_COMPONENTS_PANEL_GROUP_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include "panel.h"

namespace Antares::Component
{
/*!
** \brief Panel implementation
*/
class PanelGroup: public Panel
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

} // namespace Antares::Component

#endif // __ANTARES_COMMON_COMPONENTS_PANEL_PANEL_H__
