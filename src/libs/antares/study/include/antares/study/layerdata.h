// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_LAYERDATA_H__
#define __ANTARES_LIBS_STUDY_LAYERDATA_H__

#include <filesystem>
#include <map>
#include <string>

#include <yuni/yuni.h>

namespace Antares::Data
{
/*!
** \brief Antares Study
*/

class LayerData
{
public:
    LayerData(size_t activeLayer, bool showLayer):
        activeLayerID(activeLayer),
        showAllLayer(showLayer)
    {
    }

    //! \name Layers
    //@{
    //! All available layers
    std::map<size_t, std::string> layers;
    //@}
    size_t activeLayerID;
    bool showAllLayer;

protected:
    bool saveLayers(const AnyString& filename);
    void loadLayers(const std::filesystem::path& filename);
};

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_LAYERDATA_H__ */
