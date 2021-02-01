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

#include <yuni/yuni.h>
#include "resources.h"
#include <antares/resources/resources.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <vector>
#include <antares/logs.h>

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
