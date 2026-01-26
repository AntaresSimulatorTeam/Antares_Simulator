// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_ABOUTBOX_ABOUTBOX_H__
#define __ANTARES_APPLICATION_ABOUTBOX_ABOUTBOX_H__

#include <wx-wrapper.h>
#include <wx/dialog.h>

namespace Antares::Window
{
class AboutBox final: public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    AboutBox(wxWindow* parent);

    //! Destructor
    virtual ~AboutBox()
    {
    }

    //@}

private:
    //! Event: the user requested to close the window
    void onClose(void*);

}; // class AboutBox

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_ABOUTBOX_ABOUTBOX_H__
