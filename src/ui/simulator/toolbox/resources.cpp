// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


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

namespace Antares::Resources
{
//! Temporary string
static String location;

wxString WxFindFile(const AnyString& filename)
{
    static Yuni::String location;
    if (Resources::FindFile(location, filename))
    {
        return wxStringFromUTF8(location);
    }
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
            {
                return b;
            }
            logs.error() << "Invalid bitmap " << location;
            delete b;
        }
        else
        {
            logs.error() << "Impossible to load " << location;
        }
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
        {
            return new wxStaticBitmap(parent,
                                      id,
                                      wxBitmap(img),
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      wxTRANSPARENT_WINDOW);
        }

        logs.error() << "Impossible to load " << location;
    }
    return new wxStaticBitmap(parent,
                              id,
                              wxBitmap(),
                              wxDefaultPosition,
                              wxDefaultSize,
                              wxTRANSPARENT_WINDOW);
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
        {
            return new wxBitmapButton(parent,
                                      id,
                                      wxBitmap(img),
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      wxNO_BORDER);
        }

        logs.error() << "Impossible to load " << location;
    }
    return new wxBitmapButton(parent,
                              id,
                              wxBitmap(),
                              wxDefaultPosition,
                              wxDefaultSize,
                              wxNO_BORDER);
}

} // namespace Antares::Resources
