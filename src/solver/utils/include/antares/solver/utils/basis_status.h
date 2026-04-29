// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

namespace Test
{
class BasisStatus;
}

namespace operations_research
{
class MPSolver;
}

namespace Antares::Optimization
{
class BasisStatusImpl;

class BasisStatus
{
public:
    // Prevent copy & move
    BasisStatus();
    ~BasisStatus();
    BasisStatus(const BasisStatus&) = delete;
    BasisStatus(BasisStatus&&) = delete;
    BasisStatus& operator=(const BasisStatus&) = delete;
    BasisStatus& operator=(BasisStatus&&) = delete;

    bool exists() const;
    void setStartingBasis(operations_research::MPSolver* solver) const;
    void extractBasis(const operations_research::MPSolver* solver);
    void clear();

private:
    std::unique_ptr<BasisStatusImpl> impl;
    friend class Test::BasisStatus; // For tests
};
} // namespace Antares::Optimization
