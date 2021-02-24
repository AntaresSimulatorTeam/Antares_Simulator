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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_H__
#define __ANTARES_TOOLBOX_COMPONENTS_H__

#include <antares/wx-wrapper.h>
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
