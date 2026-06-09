// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Antares::Solver::Simulation
{
class IStorageForRemix
{
public:
    IStorageForRemix(std::string name):
        name_(name)
    {
    }

    const std::string& name() const
    {
        return name_;
    }

    virtual ~IStorageForRemix() = default;
    virtual double maxExchange(unsigned hourOfMaxGen, unsigned hourOfMinGen) const = 0;
    virtual void update() = 0;
    virtual const std::vector<double>& initWithdrawal() const = 0;
    virtual std::vector<double>& withdrawal() = 0;

private:
    virtual void checkInput(size_t size) = 0;
    std::string name_;
};

using ListStorageForRemix = std::vector<std::shared_ptr<IStorageForRemix>>;

class StorageListSort final
{
public:
    void add(const double capacity, const std::shared_ptr<IStorageForRemix> sts);
    ListStorageForRemix makeSortedList();

private:
    std::vector<std::pair<double, std::shared_ptr<IStorageForRemix>>> pairs_capa_storage_;
};

} // namespace Antares::Solver::Simulation
