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


#include <yuni/core/string.h>
#include <yuni/yuni.h>
#include <fstream>

#include "../inifile/inifile.h"
#include "layerdata.h"
#include "../logs.h"

using namespace Yuni;

namespace Antares::Data
{

void LayerData::loadLayers(const AnyString& filename)
{
    IniFile ini;
    if (std::ifstream(filename.c_str()).good()) // check if file exists
        if (ini.open(filename))
        {
            // The section
            auto* section = ini.find("layers");
            if (section)
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
                        showAllLayer = p->value.to<bool>();
                }
                return;
            }

            logs.warning() << ": The section `layers` can not be found";
            return;
        }
}

bool LayerData::saveLayers(const AnyString& filename)
{
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        CString<256, true> data;
        data << "[layers]\n";
        for (std::map<size_t, std::string>::iterator iterator = layers.begin();
             iterator != layers.end();
             iterator++)
        {
            data << iterator->first << " = " << iterator->second;
            data << '\n';
        }

        data << "[activeLayer]\n";
        data << "activeLayerID"
             << " = " << activeLayerID;
        data << '\n';
        data << "showAllLayer"
             << " = " << showAllLayer;

        file << data;

        return true;
    }

    logs.error() << "I/O error: impossible to write " << filename;
    return false;
}

} // namespace Antares::Data
