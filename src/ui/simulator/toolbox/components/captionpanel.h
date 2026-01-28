// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__
#define __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__

#include <ui/common/component/panel.h>

namespace Antares::Component
{
/*!
** \brief A simple panel with a caption
*/
class CaptionPanel: public Panel
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

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENTS_CAPTIONPANEL_H__
