/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster;
}

namespace Antares::Data::ScenarioBuilder
{
class ShortTermInflowsTSNumberData: public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void saveToINIFile(Yuni::IO::File::Stream& file) const;

    void setTSnumber(const ShortTermStorage::STStorageCluster* sts, unsigned year, unsigned value);
    unsigned get(const ShortTermStorage::STStorageCluster* sts, unsigned year) const;

private:
    std::map<const ShortTermStorage::STStorageCluster*, MatrixType> rules_;
    const Area* pArea{nullptr};
};

inline CString<512, false> ShortTermInflowsTSNumberData::get_prefix() const
{
    return "sts,";
}

} // namespace Antares::Data::ScenarioBuilder
