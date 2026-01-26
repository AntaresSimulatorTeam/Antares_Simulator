// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_WINDOW_H__
#define __ANTARES_WINDOWS_OUTPUT_WINDOW_H__

#include "output.h"
#include <ui/common/component/frame/local-frame.h>

namespace Antares::Window::OutputViewer
{
/*!
** \brief Window for output viewer
*/
class Window: public Antares::Component::Frame::WxLocalFrame
{
public:
    // \name Constructors & Destructor
    //@{
    /*!
    ** \brief Constructor with a parent window and a study
    */
    Window(wxFrame* parent, const OutputViewer::Component* viewer);
    //! Destructor
    virtual ~Window();
    //@}

private:
    /*!
    ** \brief The Window has been closed
    */
    void onClose(wxCloseEvent& evt);

private:
    // Event Table
    DECLARE_EVENT_TABLE()

}; // class Window

} // namespace Antares::Window::OutputViewer

#endif // __ANTARES_WINDOWS_OUTPUT_WINDOW_H__
