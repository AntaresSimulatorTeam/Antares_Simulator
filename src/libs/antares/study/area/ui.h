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
#ifndef __ANTARES_LIBS_STUDY_UI_H__
# define __ANTARES_LIBS_STUDY_UI_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>


namespace Antares
{
namespace Data
{

	/*!
	** \brief Information about the Area for the UI
	*/
	class AreaUI final
	{
	public:
		//! Default constructor
		AreaUI();

		//! Default constructor
		~AreaUI()
		{
			mapLayersVisibilityList.clear();
		}

		/*!
		** \brief (Re)Initialize all properties
		*/
		void reset();

		/*!
		** \brief Load settings from an INI file
		*/
		bool loadFromFile(const AnyString& filename);

		/*!
		** \brief Save the settings into a file
		**
		** \param filename Filename
		** \param force True to not rely on the modifier flag
		*/
		bool saveToFile(const AnyString& filename, bool force = false) const;

		/*!
		** \brief Amount of memory consummed by the instance
		*/
		Yuni::uint64 memoryUsage() const;

		/*!
		** \brief Get if the structure has been modified
		*/
		bool modified() const;

		/*!
		** \brief Mark the structure as modified
		*/
		void markAsModified();

		void rebuildCache();

	public:
		//! The X-Coordinate
		int x;
		std::map<size_t, int> layerX;
		//! The Y-Coordinate
		int y;
		std::map<size_t, int> layerY;

		//! Color of the area (RGB)
		int color[3];
		std::map<size_t, int[3]> layerColor;

		//! HSV color model (only computed from the interface)
		// \see UIRuntimeInfo::reload()
		Yuni::CString<12,false> cacheColorHSV;

		//! The list of layers this Node will appear on
		std::vector<size_t> mapLayersVisibilityList;

	private:
		//! Modifier flag
		mutable bool pModified;

	}; // class AreaUI





} // namespace Data
} // namespace Antares

# include "ui.hxx"

#endif // __ANTARES_LIBS_STUDY_UI_H__
