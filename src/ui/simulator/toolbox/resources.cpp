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

#include <yuni/yuni.h>
#include "resources.h"
#include "wx-wrapper.h"
#include <antares/resources/resources.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <vector>
#include <antares/logs/logs.h>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Resources
{
//! Temporary string
static String location;

wxString WxFindFile(const AnyString& filename)
{
    static Yuni::String location;
    if (Resources::FindFile(location, filename))
        return wxStringFromUTF8(location);
    return wxEmptyString;
}

wxBitmap* BitmapLoadFromFile(const char* filename)
{
    // Assert
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (filename && FindFile(location, filename))
    {
        wxImage img;
        const wxString& wxloc = wxStringFromUTF8(location);
        if (img.LoadFile(wxloc))
        {
            wxBitmap* b = new wxBitmap(img);
            if (b->IsOk())
                return b;
            logs.error() << "Invalid bitmap " << location;
            delete b;
        }
        else
            logs.error() << "Impossible to load " << location;
    }
    return nullptr;
}

wxStaticBitmap* StaticBitmapLoadFromFile(wxWindow* parent,
                                         const wxWindowID id,
                                         const char* filename)
{
    // Assert
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (filename && Resources::FindFile(location, filename))
    {
        wxImage img;
        if (img.LoadFile(wxStringFromUTF8(location)))
            return new wxStaticBitmap(
              parent, id, wxBitmap(img), wxDefaultPosition, wxDefaultSize, wxTRANSPARENT_WINDOW);

        logs.error() << "Impossible to load " << location;
    }
    return new wxStaticBitmap(
      parent, id, wxBitmap(), wxDefaultPosition, wxDefaultSize, wxTRANSPARENT_WINDOW);
}

wxBitmapButton* BitmapButtonLoadFromFile(wxWindow* parent,
                                         const wxWindowID id,
                                         const char* filename)
{
    // Assert
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    if (filename && Resources::FindFile(location, filename))
    {
        wxImage img;
        if (img.LoadFile(wxStringFromUTF8(location)))
            return new wxBitmapButton(
              parent, id, wxBitmap(img), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

        logs.error() << "Impossible to load " << location;
    }
    return new wxBitmapButton(
      parent, id, wxBitmap(), wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
}

} // namespace Resources
} // namespace Antares
