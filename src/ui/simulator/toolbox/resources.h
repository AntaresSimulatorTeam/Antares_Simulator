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
#ifndef __ANTARES_TOOLBOX_RESOURCES_H__
#define __ANTARES_TOOLBOX_RESOURCES_H__

#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include <wx/bmpbuttn.h>
#include <antares/resources/resources.h>

namespace Antares
{
namespace Resources
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

} // namespace Resources
} // namespace Antares

#endif // __ANTARES_TOOLBOX_RESOURCES_H__
