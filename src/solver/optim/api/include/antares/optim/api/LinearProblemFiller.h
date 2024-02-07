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

#include "LinearProblem.h"
#include "LinearProblemData.h"
using namespace operations_research;

namespace Antares::optim::api
{
    class LinearProblemFiller
    {
    public:
        // TODO : remplacer pointeurs par références
        virtual void addVariables(LinearProblem& problem, const LinearProblemData& data) = 0;
        virtual void addConstraints(LinearProblem& problem, const LinearProblemData& data) = 0;
        virtual void addObjective(LinearProblem& problem, const LinearProblemData& data) = 0;
        virtual void update(LinearProblem& problem, const LinearProblemData& data) = 0; // TODO : comment autoriser de màj seulement les coefs et lb/ub ? ne pas ajouter de contrainte, variable ...
        // update nécessaire pour mettre à jour les données de semaine en semaine
        // la structure de données hebdo s'appelle SIM, c'est elle qui est utilisée pour mettre à jour le problème hebdo
        // sim_structure_contrainte_economique.h
        // ApportNaturelHoraire
        // la structure de données devra devenir générique, au moins au niveau API standard (dictionnaire)
    };
}
