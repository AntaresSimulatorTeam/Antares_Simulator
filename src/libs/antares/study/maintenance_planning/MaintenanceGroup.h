/*
** Copyright 2007-2023 RTE
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
#pragma once

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include "../fwd.h"
#include "../../constants.h"

#include "../parts/thermal/cluster.h"
#include <antares/array/matrix.h>
#include <antares/inifile/inifile.h>

#include "antares/study/filter.h"

#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <antares/utils/utils.h>
#include "antares/study/area/area.h"
#include <filesystem>

namespace Antares::Data
{

class MaintenanceGroup : public Yuni::NonCopyable<MaintenanceGroup>
{
    using areaName = std::string;
    using mapAreas = std::map<areaName, Area*>;
    using mapThermalClusters = std::map<ThermalCluster*, bool>;
public:
    void name(const std::string& newname);
    uint getNbOfAssociatedAreas() const;
    bool isEnabled() const;
    bool saveAreaListToFile(const std::string& filename) const;


    mapAreas associatedAreas;
    mapThermalClusters associatedThermalClusters;

private:
    std::string name_;
    std::string ID_;



    bool enabled_;

};

} // namespace Antares::Data