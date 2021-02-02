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
#ifndef __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__
#define __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__

#include <antares/wx-wrapper.h>
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Component
{
/*!
** \brief A simple panel with a caption
*/
class CaptionPanel : public Panel
{
public:
    //! \name Constructors & Destructor
    //@{
    /*!
    ** \brief Default contructor
    ** \param parent The parent control
    */
    CaptionPanel(wxWindow* parent);
    /*!
    ** \brief Constructor
    ** \param parent The parent control
    ** \param caption Caption of the panel
    */
    CaptionPanel(wxWindow* parent, const wxString& caption);

    //! Destructor
    ~CaptionPanel();
    //@}

    //! \name Caption
    //@{
    //! Get the caption of the panel
    const wxString& caption() const
    {
        return pCaption;
    }
    //! Set the caption of the panel
    void caption(const wxString& s);
    //@}

protected:
    /*!
    ** \brief Event: The panel has to paint itself
    */
    void onDraw(wxPaintEvent& evt);
    /*!
    ** \brief Event: The panel is been resizing
    */
    void onResize(wxSizeEvent& evt);

private:
    //! Caption of the panel
    wxString pCaption;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class CaptionPanel

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__
