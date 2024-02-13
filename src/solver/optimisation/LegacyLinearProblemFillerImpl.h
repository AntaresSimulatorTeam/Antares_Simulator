/*
** Copyright 2007-2024 RTE
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

#include "optim/api/include/antares/optim/api/LinearProblemFiller.h"
#include "utils/include/antares/solver/utils/named_problem.h"
#include "LegacyLinearProblemImpl.h"

class LegacyLinearProblemFillerImpl final : public Antares::optim::api::LinearProblemFiller
{
private:
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* legacyProblem_;
    static void declareBalanceConstraints(LegacyLinearProblemImpl* legacyLinearProblem, const LinearProblemData::Legacy& legacy);
public:
    explicit LegacyLinearProblemFillerImpl(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* legacyProblem) : legacyProblem_(legacyProblem) {};
    void addVariables(Antares::optim::api::LinearProblem& problem, const Antares::optim::api::LinearProblemData& data) override;
    void addConstraints(Antares::optim::api::LinearProblem& problem, const Antares::optim::api::LinearProblemData& data) override;
    void addObjective(Antares::optim::api::LinearProblem& problem, const Antares::optim::api::LinearProblemData& data) override;
    void update(Antares::optim::api::LinearProblem& problem, const Antares::optim::api::LinearProblemData& data) override;
};
