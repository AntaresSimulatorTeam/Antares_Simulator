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
#ifndef __ANTARES_TOOLBOX_COMPONENT_MAIN_PANEL_H__
#define __ANTARES_TOOLBOX_COMPONENT_MAIN_PANEL_H__

#include <antares/wx-wrapper.h>
#include <wx/dc.h>
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Component
{
class MainPanel final : public Panel, public Yuni::IEventObserver<MainPanel>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    ** \param parent The parent control
    */
    MainPanel(wxWindow* parent);
    //! Destructor
    virtual ~MainPanel();
    //@}

    //! \name Caption
    //@{
    //! Get the caption/title of the study
    const wxString& studyCaption() const
    {
        return pStudyCaption;
    }
    //! Set the caption/title of the study
    void studyCaption(const wxString& s);
    //@}

    //! \name Author
    //@{
    //! Get the author
    const wxString& author() const
    {
        return pAuthor;
    }
    //! Set the author
    void author(const wxString& s);
    //@}

    void refreshFromStudy();

private:
    /*!
    ** \brief Event: The panel has to draw itself
    */
    void onDraw(wxPaintEvent& evt);

    /*!
    ** \brief UI Event: Erase background
    */
    void onEraseBackground(wxEraseEvent&)
    {
    }

    /*!
    ** \brief Add a property (pair key/value)
    **
    ** \param dc      The device context
    ** \param caption The caption of the property
    ** \param text    The text associated to the property
    */
    void addProperty(wxDC& dc,
                     const wxString& caption,
                     const wxString& text,
                     const wxSize& size,
                     int& posY) const;

    void onStudyChanged(Antares::Data::Study& study);
    void onStudyChanged();

private:
    //! Caption
    wxString pStudyCaption;
    //! Author
    wxString pAuthor;
    //! The best height for the panel
    int pCachedSizeY;
    //! Beta image
    wxBitmap* pBeta;
    //! RC image
    wxBitmap* pRC;

    DECLARE_EVENT_TABLE()

}; // class MainPanel

} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_MAIN_PANEL_H__
