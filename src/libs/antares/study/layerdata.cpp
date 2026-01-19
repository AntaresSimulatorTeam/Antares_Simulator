// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/layerdata.h"

#include <fstream>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares::Data
{
void LayerData::loadLayers(const std::filesystem::path& filename)
{
    IniFile ini;
    if (std::ifstream(filename.c_str()).good() && ini.open(filename)) // check if file exists
    {
        // The section
        if (auto* section = ini.find("layers"); section)
        {
            size_t key;
            CString<50, false> value;

            for (auto* p = section->firstProperty; p; p = p->next)
            {
                // We convert the key and the value into the lower case format,
                // since several tests will be done with these string */
                key = p->key.to<size_t>();
                value = p->value;

                layers[key] = value.to<std::string>();
            }

            section = ini.find("activeLayer");
            if (section)
            {
                auto* p = section->firstProperty;
                activeLayerID = p->value.to<size_t>();

                p = p->next;

                if (p)
                {
                    showAllLayer = p->value.to<bool>();
                }
            }
            return;
        }

        logs.warning() << ": The section `layers` can not be found";
        return;
    }
}

bool LayerData::saveLayers(const AnyString& filename)
{
    if (IO::File::Stream file; file.openRW(filename))
    {
        CString<256, true> data;
        data << "[layers]\n";

        for (const auto& [key, value]: layers)
        {
            data << key << " = " << value << '\n';
        }

        data << "[activeLayer]\n";
        data << "activeLayerID" << " = " << activeLayerID;
        data << '\n';
        data << "showAllLayer" << " = " << showAllLayer;

        file << data;

        return true;
    }

    logs.error() << "I/O error: impossible to write " << filename;
    return false;
}

} // namespace Antares::Data
