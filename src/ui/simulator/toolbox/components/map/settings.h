// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_SETTINGS_H__
#define __ANTARES_TOOLBOX_MAP_SETTINGS_H__

#include <wx/colour.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <list>
#include <cstdint>

namespace Antares::Map
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

} // namespace Antares::Map

#endif // __ANTARES_TOOLBOX_MAP_SETTINGS_H__
