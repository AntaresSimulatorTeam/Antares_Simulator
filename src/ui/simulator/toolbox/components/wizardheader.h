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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_H__
#define __ANTARES_TOOLBOX_COMPONENTS_H__

#include <ui/common/component/panel.h>

namespace Antares
{
namespace Toolbox
{
namespace Components
{
class WizardHeader : public Antares::Component::Panel
{
public:
    /*!
    ** \brief Create a new instance
    */
    static Antares::Component::Panel* Create(wxWindow* parent,
                                             const wxString& caption,
                                             const char* img,
                                             const wxString& help = wxEmptyString,
                                             const int wrap = 500,
                                             bool customDraw = true);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    WizardHeader(wxWindow* parent, const char* icon, const wxString& text, const wxString& sub);
    //! Destructor
    virtual ~WizardHeader();
    //@}

    /*!
    ** \brief Event: The panel has to draw itself
    */
    virtual void onDraw(wxPaintEvent& evt);

    /*!
    ** \brief UI Event: Erase background
    */
    virtual void onEraseBackground(wxEraseEvent&)
    {
    }

public:
    int sizeNeeded;

private:
    void onDraw();

    wxString pText;
    wxString pSubText;
    wxBitmap* pIcon;

    DECLARE_EVENT_TABLE()

}; // class WizardHeader

} // namespace Components
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_H__
