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
#ifndef __ANTARES_TOOLBOX_MAP_SETTINGS_H__
#define __ANTARES_TOOLBOX_MAP_SETTINGS_H__

#include <wx/colour.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <list>
#include <cstdint>

namespace Antares
{
namespace Map
{
enum mapImageFormat : uint16_t
{
    mfPNG,
    mfDefaultFormat = 0,
    mfJPG,

    //
    mfSVG,

    //--
    mfFormatCount
};

struct MapRenderOptions
{
    bool mapInFile;
    mapImageFormat fileFormat;
    bool transparentBackground;
    wxColor backgroundColor;
    uint16_t nbTiles;
    std::list<uint16_t> layers;
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
