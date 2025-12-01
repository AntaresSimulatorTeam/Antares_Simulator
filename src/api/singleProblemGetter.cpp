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

#include "antares/api/singleProblemGetter.h"

#include "singleProblemGetterImpl.h"

namespace Antares::Solver
{
SingleProblemGetter::SingleProblemGetter(const std::filesystem::path& studyPath):
    impl_(std::make_unique<Implementation::SingleProblemGetter>(studyPath))
{
}

/* std::unique_ptr's destructor does not handle incomplete types
   Implementation::SingleProblemGetter is complete here */
SingleProblemGetter::~SingleProblemGetter() = default;

ConstantDataFromAntares SingleProblemGetter::getConstantData()
{
    return impl_->getConstantData();
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    return impl_->getWeeklyData(id);
}

} // namespace Antares::Solver
