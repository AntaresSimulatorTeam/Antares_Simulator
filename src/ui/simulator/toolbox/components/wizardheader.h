// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_H__
#define __ANTARES_TOOLBOX_COMPONENTS_H__

#include <ui/common/component/panel.h>

namespace Antares::Toolbox::Components
{
class WizardHeader: public Antares::Component::Panel
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

} // namespace Antares::Toolbox::Components

#endif // __ANTARES_TOOLBOX_COMPONENTS_H__
