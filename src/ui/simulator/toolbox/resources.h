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
#ifndef __ANTARES_TOOLBOX_RESOURCES_H__
# define __ANTARES_TOOLBOX_RESOURCES_H__

# include <antares/wx-wrapper.h>
# include <wx/bitmap.h>
# include <wx/statbmp.h>
# include <wx/bmpbuttn.h>
# include <antares/resources/resources.h>



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
	wxStaticBitmap* StaticBitmapLoadFromFile(wxWindow* parent, const wxWindowID id,
		const char* filename);

	/*!
	** \brief Create a new instance of `wxBitmapButton` from a resource image file
	**
	** \param parent The parent window
	** \param id The id for the new control
	** \param reFilename The filename, relative to the resource folder
	** \return A new instance of a wxStaticBitmap
	*/
	wxBitmapButton* BitmapButtonLoadFromFile(wxWindow* parent, const wxWindowID id,
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
