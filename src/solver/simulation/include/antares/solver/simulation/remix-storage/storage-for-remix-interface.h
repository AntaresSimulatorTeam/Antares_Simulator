#pragma once

#include <memory>
#include <vector>

namespace Antares::Solver::Simulation
{
class IStorageForRemix
{
public:
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
