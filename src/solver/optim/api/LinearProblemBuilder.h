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

#include "LinearProblemFiller.h"
using namespace std;

namespace Antares::optim::api
{
    class LinearProblemBuilder
    {
    private:
        LinearProblem* linearProblem_;
        vector<LinearProblemFiller*> fillers_;
        bool built = false;
    public:
        explicit LinearProblemBuilder(LinearProblem* linearProblem) : linearProblem_(linearProblem) {};

        void addFiller(LinearProblemFiller* filler)
        {
            fillers_.push_back(filler);
        }

        void build() {
            if (built) {
                // TODO : throw
            }
            for (auto filler : fillers_)
            {
                filler->addVariables(linearProblem_);
            }
            for (auto filler : fillers_)
            {
                filler->addConstraints(linearProblem_);
            }
            for (auto filler : fillers_)
            {
                filler->addObjective(linearProblem_);
            }
        }

        MipSolution solve()
        {
            // TODO : move to new interface LinearProblemSolver ??
            if (!built) {
                build();
            }
            return linearProblem_->solve();
        }
    };
}