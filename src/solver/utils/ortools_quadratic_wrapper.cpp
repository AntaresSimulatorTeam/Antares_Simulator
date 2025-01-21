/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <absl/status/statusor.h>
#include <ortools/math_opt/cpp/math_opt.h>
#include <pi_constantes_externes.h>

#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_quadratic_wrapper.h>
#include <antares/solver/utils/ortools_utils.h>

using Antares::Solver::Optimization::OptimizationOptions;
using namespace operations_research::math_opt;

constexpr double kInf = std::numeric_limits<double>::infinity();

void BuildVariablesAndObjective(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model);
void BuildConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model);

// TODO: interpret the following lines sent to sirius & adapt them to mathopt if needed
//     Probleme->VariableBinaire = (char*)ProblemeAResoudre.CoutsReduits.data();
//     Probleme->UtiliserLaToleranceDAdmissibiliteParDefaut = OUI_PI;
//     Probleme->UtiliserLaToleranceDeStationnariteParDefaut = OUI_PI;
//     Probleme->UtiliserLaToleranceDeComplementariteParDefaut = OUI_PI;
//     Probleme->CoutsMarginauxDesContraintes =ProblemeAResoudre.CoutsMarginauxDesContraintes.data()

void checkOptions(const OptimizationOptions& options)
{
    auto availableSolversList = getAvailableSolverNames(SolverClass::QUADRATIC);
    bool solverFound = std::ranges::find(availableSolversList, options.quadraticSolver)
                       != availableSolversList.end();
    if (!solverFound)
    {
        throw std::invalid_argument("Solver " + options.quadraticSolver
                                    + " not supported for quadratic problems optimization.");
    }
    if (!options.quadraticSolverParameters.empty())
    {
        // TODO: handle these by mapping them to generic or solver-specific params in mathopt
        // TODO: or remove this for now?
        Antares::logs.warning()
          << "Quadratic solver parameters are not supported yet; they will be ignored.";
    }
}

void ProcessSolveResult(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                        Model& model,
                        absl::StatusOr<SolveResult> resultStatus);

void SolveQuadraticProblemWithOrtools(const OptimizationOptions& options,
                                      PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    checkOptions(options);
    Model model("Quadratic problem");
    BuildVariablesAndObjective(ProblemeAResoudre, model);
    BuildConstraints(ProblemeAResoudre, model);
    SolveArguments args;
    args.parameters.enable_output = true;
    if (options.solverLogs)
    {
        args.parameters.enable_output = true;
    }
    auto solverType = OrtoolsUtils::mathoptSolverMap.at(options.quadraticSolver);
    auto resultStatus = Solve(model, solverType, args);
    ProcessSolveResult(ProblemeAResoudre, model, resultStatus);
}

void BuildVariablesAndObjective(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model)
{
    QuadraticExpression objective(0);
    for (auto i = 0; i < ProblemeAResoudre->NombreDeVariables; ++i)
    {
        double lb, ub;
        switch (ProblemeAResoudre->TypeDeVariable[i])
        {
        case VARIABLE_BORNEE_DES_DEUX_COTES:
            lb = ProblemeAResoudre->Xmin[i];
            ub = ProblemeAResoudre->Xmax[i];
            break;
        case VARIABLE_BORNEE_INFERIEUREMENT:
            lb = ProblemeAResoudre->Xmin[i];
            ub = kInf;
            break;
        case VARIABLE_BORNEE_SUPERIEUREMENT:
            lb = -kInf;
            ub = ProblemeAResoudre->Xmax[i];
            break;
        case VARIABLE_NON_BORNEE:
            lb = -kInf;
            ub = kInf;
            break;
        default:
            throw std::invalid_argument("Unknown variable type: "
                                        + std::to_string(ProblemeAResoudre->TypeDeVariable[i]));
        }
        std::string name = ProblemeAResoudre->NomDesVariables[i].empty()
                             ? "C" + std::to_string(i)
                             : ProblemeAResoudre->NomDesVariables[i];
        auto var = model.AddVariable(lb, ub, ProblemeAResoudre->VariablesEntieres[i], name);
        objective += QuadraticExpression(
          {QuadraticTerm(var, var, ProblemeAResoudre->CoutQuadratique[i])},
          {LinearTerm(var, ProblemeAResoudre->CoutLineaire[i])},
          0.0);
    }
    model.SetObjective(objective, false);
}

void BuildConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model)
{
    for (auto iCt = 0; iCt < ProblemeAResoudre->NombreDeContraintes; ++iCt)
    {
        LinearExpression linear_expression(0);
        for (auto iCoef = 0; iCoef < ProblemeAResoudre->NombreDeTermesDesLignes[iCt]; ++iCoef)
        {
            int iVar = ProblemeAResoudre->IndicesColonnes
                         .data()[ProblemeAResoudre->IndicesDebutDeLigne[iCt] + iCoef];
            auto coef = ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes
                          .data()[ProblemeAResoudre->IndicesDebutDeLigne[iCt] + iCoef];
            linear_expression += model.variable(iVar) * coef;
        }
        double lb = -kInf;
        double ub = kInf;
        switch (ProblemeAResoudre->Sens[iCt])
        {
        case '=':
            lb = ub = ProblemeAResoudre->SecondMembre[iCt];
            break;
        case '<':
            ub = ProblemeAResoudre->SecondMembre[iCt];
            break;
        case '>':
            lb = ProblemeAResoudre->SecondMembre[iCt];
            break;
        default:
            throw std::invalid_argument("Expected constraint sense to be =, <, or >, but was: "
                                        + ProblemeAResoudre->Sens[iCt]);
        }
        BoundedLinearExpression bounded_linear_expression(std::move(linear_expression), lb, ub);
        std::string name = ProblemeAResoudre->NomDesContraintes[iCt].empty()
                             ? "R" + std::to_string(iCt)
                             : ProblemeAResoudre->NomDesContraintes[iCt];
        model.AddLinearConstraint(bounded_linear_expression, name);
    }
}

void ProcessSolveResult(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                        Model& model,
                        absl::StatusOr<SolveResult> resultStatus)
{
    if (resultStatus.ok() && resultStatus.value().has_primal_feasible_solution())
    {
        ProblemeAResoudre->ExistenceDUneSolution = OUI_PI;
        // Store result in problem structure
        auto result = resultStatus.value();
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; ++i)
        {
            auto var = model.variable(i);
            // Primal results
            double value = result.best_primal_solution().variable_values.at(var);
            ProblemeAResoudre->X[i] = value;
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = value;
            }
            // Reduced costs
            if (result.has_dual_feasible_solution())
            {
                ProblemeAResoudre->CoutsReduits[i] = result.reduced_costs().at(var);
            }
        }
    }
    else
    {
        ProblemeAResoudre->ExistenceDUneSolution = NON_PI;
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; ++i)
        {
            ProblemeAResoudre->X[i] = std::numeric_limits<double>::quiet_NaN();
            double* pt = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i];
            if (pt)
            {
                *pt = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
}
