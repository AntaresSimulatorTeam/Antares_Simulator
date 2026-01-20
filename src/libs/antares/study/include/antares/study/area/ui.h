// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_UI_H__
#define __ANTARES_LIBS_STUDY_UI_H__

#include <map>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares::Data
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

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_UI_H__
