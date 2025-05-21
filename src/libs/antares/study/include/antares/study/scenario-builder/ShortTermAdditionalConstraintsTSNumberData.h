/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
class AdditionalConstraints;
}

namespace Antares::Data::ScenarioBuilder
{
class ShortTermAdditionalConstraintsTSNumberData: public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

    bool reset(const Study& study) override;

    void attachArea(Area* area)
    {
        pArea = area;
    }

    void saveToINIFile(Yuni::IO::File::Stream& file) const;

    void setTSnumber(const ShortTermStorage::AdditionalConstraints* ct,
                     unsigned year,
                     unsigned value);
    unsigned get(const ShortTermStorage::AdditionalConstraints* ct, unsigned year) const;

private:
    std::map<const ShortTermStorage::AdditionalConstraints*, MatrixType> rules_;
    Area* pArea;
};

inline CString<512, false> ShortTermAdditionalConstraintsTSNumberData::get_prefix() const
{
    return "sta,";
}

} // namespace Antares::Data::ScenarioBuilder
