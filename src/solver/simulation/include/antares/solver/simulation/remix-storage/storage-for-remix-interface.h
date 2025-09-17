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

#include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{
class IStorageForRemix
{
public:
    virtual ~IStorageForRemix() = default;
    virtual double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const = 0;
    virtual void update() = 0;
    virtual const std::vector<double>& initWithdrawal() const = 0;
    virtual std::vector<double>& withdrawal() = 0;

private:
    virtual void checkInput(size_t size) = 0;
};

using ListStorageForRemix = std::vector<std::shared_ptr<IStorageForRemix>>;

class StorageListSort
{
public:
    void add(const double capacity, const std::shared_ptr<IStorageForRemix> sts);
    ListStorageForRemix makeSortedList();

private:
    std::vector<std::pair<double, std::shared_ptr<IStorageForRemix>>> pairs_capa_storage_;
};

} // namespace Antares::Solver::Simulation
