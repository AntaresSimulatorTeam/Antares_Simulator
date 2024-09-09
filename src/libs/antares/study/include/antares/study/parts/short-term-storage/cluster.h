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

#pragma once
#include <string>
#include <memory>
#include <antares/inifile/inifile.h>
#include "properties.h"
#include "series.h"
#include <antares/study/area/capacityReservation.h>

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster
{
public:
    //! \brief Get the group name string
    static const char* GroupName(enum Group grp);

    bool enabled() const;
    bool validate() const;

    bool loadFromSection(const IniFile::Section& section);
    bool loadSeries(const std::string& folder) const;

    void saveProperties(IniFile& ini) const;
    bool saveSeries(const std::string& path) const;

    void addReserveParticipation(Data::ReserveName name,
                                 STStorageClusterReserveParticipation& reserveParticipation);

    //! \brief Returns max turbining power for a reserve if participating, -1 otherwise
    float reserveMaxTurbining(Data::ReserveName name);
    
    //! \brief Returns max pumping power for a reserve if participating, -1 otherwise
    float reserveMaxPumping(Data::ReserveName name);

    //! \brief Returns participating cost for a reserve if participating, -1 otherwise
    float reserveCost(Data::ReserveName name);

    //! \brief Returns the number of reserve the cluster is participating to
    uint reserveParticipationsCount();

    std::string id;

    std::shared_ptr<Series> series = std::make_shared<Series>();
    mutable Properties properties;

    //! reserve
    std::unordered_map<Data::ReserveName, STStorageClusterReserveParticipation> clusterReservesParticipations;
};
} // namespace Antares::Data::ShortTermStorage
