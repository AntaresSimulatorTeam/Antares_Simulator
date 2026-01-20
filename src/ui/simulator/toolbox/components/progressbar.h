// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__
#define __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__

#include <ui/common/component/panel.h>

namespace Antares::Component
{
/*!
** \brief A simple panel with a caption
*/
class ProgressBar: public Panel
{
public:
    //! \name Constructors & Destructor
    //@{
    /*!
    ** \brief Default contructor
    ** \param parent The parent control
    */
    ProgressBar(wxWindow* parent);

    //! Destructor
    ~ProgressBar();

    //@}

    //! \name Value
    //@{
    //! Get the progress value
    double value() const
    {
        return pValue;
    }

    //! Set the progress value
    void value(double v)
    {
        pValue = v;
        Refresh();
    }

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
    //! Progression
    double pValue;
    // Event table
    DECLARE_EVENT_TABLE()

}; // class ProgressBar

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENTS_PROGRESSBAR_H__
