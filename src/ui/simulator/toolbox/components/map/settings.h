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
#ifndef __ANTARES_TOOLBOX_MAP_SETTINGS_H__
# define __ANTARES_TOOLBOX_MAP_SETTINGS_H__

# include <antares/wx-wrapper.h>
# include <wx/colour.h>
# include <wx/bitmap.h>
# include <wx/image.h>

namespace Antares
{
namespace Map
{
	enum mapImageFormat : uint16_t
	{
		mfPNG, mfDefaultFormat = 0,
		mfJPG,

		//
		mfSVG,
			
		//--
		mfFormatCount
	};

	static const char* formatNames[mfFormatCount]		= {	"PNG", "JPEG", "SVG" };
	static const char* formatExt[mfFormatCount]			= { ".png", ".jpg", ".svg" };
	static wxBitmapType formatWXConstant[mfFormatCount]	= { wxBITMAP_TYPE_PNG, wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_INVALID };
	
	struct MapRenderOptions
	{
		bool			mapInFile;
		mapImageFormat	fileFormat;
		bool			transparentBackground;
		wxColor			backgroundColor;
		uint16_t		nbTiles;
		std::list<uint16_t>  layers;
	};

	class Settings
	{
	public:
		static wxColour background;

		static wxColour text;
		static wxColour textLight;

		static wxColour grid;
		static wxColour gridCenter;

		static wxColour defaultNodeBackground;
		static wxColour defaultNodeShadow;

		static wxColour selectionNodeBorder;
		static wxColour selectionNodeBackground;

		static wxColour clusterBorder;
		static wxColour clusterBackground;
		static wxColour clusterBorderSelected;
		static wxColour clusterBackgroundSelected;

		static wxColour selectionBoxBorder;
		static wxColour selectionBoxBackground;

		static wxColour selectionBoxBorderHighlighted;
		static wxColour selectionBoxBackgroundHighlighted;

		static wxColour mouseSelectionBorder;
		static wxColour mouseSelectionBackground;

		static wxColour connection;
		static wxColour connectionHighlighted;
	};




} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_SETTINGS_H__
