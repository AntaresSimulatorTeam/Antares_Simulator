// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/solver/utils/basis_status.h>

#include "basis_status_impl.h"

namespace Antares::Optimization
{
BasisStatus::BasisStatus():
    impl(std::make_unique<BasisStatusImpl>())
{
}

BasisStatus::~BasisStatus() = default;

void BasisStatus::setStartingBasis(operations_research::MPSolver* solver) const
{
    impl->setStartingBasis(solver);
}

void BasisStatus::extractBasis(const operations_research::MPSolver* solver)
{
    impl->extractBasis(solver);
}

bool BasisStatus::exists() const
{
    return impl->exists();
}

void BasisStatus::clear()
{
    impl->clear();
}

} // namespace Antares::Optimization
