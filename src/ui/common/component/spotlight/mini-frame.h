// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_MINIFRAME_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_MINIFRAME_H__

#include <yuni/yuni.h>
#include <ui/common/wx-wrapper.h>
#include <wx/minifram.h>
#include <wx/frame.h>

namespace Antares::Private::Spotlight
{
class SpotlightMiniFrame: public wxFrame
{
public:
    //! Get the current instance (if any) of the frame
    static SpotlightMiniFrame* Instance();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    SpotlightMiniFrame(wxWindow* parent);
    //! Destructor
    virtual ~SpotlightMiniFrame();
    //@}

    //! Event: the form is about to be closed
    void onClose(wxCloseEvent& evt);

    void onKillFocus(wxFocusEvent& evt);

private:
    void removeRefToMySelf();

protected:
    // Event Table
    DECLARE_EVENT_TABLE()

}; // class SpotlightMiniFrame

} // namespace Antares::Private::Spotlight

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_MINIFRAME_H__
