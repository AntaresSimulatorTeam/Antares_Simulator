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
#ifndef __ANTARES_WINDOWS_OUTPUT_MINI_FRAME_H__
#define __ANTARES_WINDOWS_OUTPUT_MINI_FRAME_H__

#include <yuni/yuni.h>
#include <ui/common/wx-wrapper.h>
#include <wx/minifram.h>

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
class SpotlightMiniFrame final : public wxMiniFrame
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

private:
    // Event Table
    DECLARE_EVENT_TABLE()

}; // class SpotlightMiniFrame

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_MINI_FRAME_H__
