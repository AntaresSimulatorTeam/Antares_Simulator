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
#ifndef __ANTARES_LIBS_STUDY_UI_H__
#define __ANTARES_LIBS_STUDY_UI_H__

#include <map>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

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
    uint64_t memoryUsage() const;

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
    Yuni::CString<12, false> cacheColorHSV;

    //! The list of layers this Node will appear on
    std::vector<size_t> mapLayersVisibilityList;

private:
    //! Modifier flag
    mutable bool pModified;

}; // class AreaUI

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_UI_H__
