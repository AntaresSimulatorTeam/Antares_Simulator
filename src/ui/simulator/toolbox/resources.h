// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_RESOURCES_H__
#define __ANTARES_TOOLBOX_RESOURCES_H__

#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include <wx/bmpbuttn.h>
#include <antares/resources/resources.h>

namespace Antares::Resources
{
/*!
** \brief Create a new instance of `wxStaticBitmap` from a resource image file
**
** \param parent The parent window
** \param id The id for the new control
** \param reFilename The filename, relative to the resource folder
** \return A new instance of a wxStaticBitmap
*/
wxStaticBitmap* StaticBitmapLoadFromFile(wxWindow* parent,
                                         const wxWindowID id,
                                         const char* filename);

/*!
** \brief Create a new instance of `wxBitmapButton` from a resource image file
**
** \param parent The parent window
** \param id The id for the new control
** \param reFilename The filename, relative to the resource folder
** \return A new instance of a wxStaticBitmap
*/
wxBitmapButton* BitmapButtonLoadFromFile(wxWindow* parent,
                                         const wxWindowID id,
                                         const char* filename);

/*!
** \brief Create a new instance of `wxBitmap` from a resource image file
**
** \param reFilename The filename, relative to the resource folder
** \return A new instance of a wxBitmap
*/
wxBitmap* BitmapLoadFromFile(const char* filename);

/*!
** \brief
**
** \warning This method must always be called from the main thread
*/
wxString WxFindFile(const AnyString& filename);

} // namespace Antares::Resources

#endif // __ANTARES_TOOLBOX_RESOURCES_H__
