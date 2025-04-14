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

#include <ortools/math_opt/cpp/math_opt.h>
#include <pi_constantes_externes.h>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_quadratic_wrapper.h>
#include <antares/solver/utils/ortools_utils.h>

using Antares::Solver::Optimization::SingleOptimOptions;
using namespace operations_research::math_opt;

constexpr double infinity = std::numeric_limits<double>::infinity();

void BuildVariablesAndObjective(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model);
void BuildConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre, Model& model);

// TODO: interpret the following lines sent to sirius & adapt them to mathopt if needed
//     Probleme->UtiliserLaToleranceDAdmissibiliteParDefaut = OUI_PI;
//     Probleme->UtiliserLaToleranceDeStationnariteParDefaut = OUI_PI;
//     Probleme->UtiliserLaToleranceDeComplementariteParDefaut = OUI_PI;

void checkOptions(const SingleOptimOptions& options)
{
    auto availableSolversList = availableQuadraticSolversList();
    bool solverFound = std::ranges::find(availableSolversList, options.solverName)
                       != availableSolversList.end();
    if (!solverFound || options.solverName.compare("sirius") == 0)
    {
        throw std::invalid_argument(
          "Solver " + options.solverName
          + " is not supported for quadratic problems optimization through MathOpt.");
    }
    if (!options.solverParameters.empty())
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

void SolveQuadraticProblemWithOrtools(const SingleOptimOptions& options,
                                      PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    checkOptions(options);
    Model model("Quadratic problem");
    BuildVariablesAndObjective(ProblemeAResoudre, model);
    BuildConstraints(ProblemeAResoudre, model);
    SolveArguments args;
    if (options.solverLogs)
    {
        args.parameters.enable_output = true;
    }
    auto solverType = OrtoolsUtils::mathoptSolverMap.at(options.solverName);
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
            ub = infinity;
            break;
        case VARIABLE_BORNEE_SUPERIEUREMENT:
            lb = -infinity;
            ub = ProblemeAResoudre->Xmax[i];
            break;
        case VARIABLE_NON_BORNEE:
            lb = -infinity;
            ub = infinity;
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
        double lb = -infinity;
        double ub = infinity;
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
                                        + ProblemeAResoudre->Sens.substr(iCt, 1));
        }
        BoundedLinearExpression bounded_linear_expression(std::move(linear_expression), lb, ub);
        std::string name = ProblemeAResoudre->NomDesContraintes[iCt].empty()
                             ? "R" + std::to_string(iCt)
                             : ProblemeAResoudre->NomDesContraintes[iCt];
        model.AddLinearConstraint(bounded_linear_expression, name);
    }
}

void FillWithValues(std::vector<double>& destination, const std::vector<double>& origin)
{
    if (destination.size() != origin.size())
    {
        throw std::invalid_argument("Expected destination and origin to have the same size");
    }
    for (unsigned i = 0; i < destination.size(); ++i)
    {
        destination[i] = origin[i];
    }
}

void FillWithValues(std::vector<double*>& destination, const std::vector<double>& origin)
{
    if (destination.size() != origin.size())
    {
        throw std::invalid_argument("Expected destination and origin to have the same size");
    }
    for (unsigned i = 0; i < destination.size(); ++i)
    {
        double* pt = destination[i];
        if (pt)
        {
            *pt = origin[i];
        }
    }
}

void FillWithNaN(std::vector<double>& vector)
{
    for (unsigned i = 0; i < vector.size(); ++i)
    {
        vector[i] = std::numeric_limits<double>::quiet_NaN();
    }
}

void FillWithNaN(std::vector<double*>& vector)
{
    for (unsigned i = 0; i < vector.size(); ++i)
    {
        double* pt = vector[i];
        if (pt)
        {
            *pt = std::numeric_limits<double>::quiet_NaN();
        }
    }
}

void ProcessSolveResult(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                        Model& model,
                        absl::StatusOr<SolveResult> resultStatus)
{
    // Store results (status, primals, duals, reduced costs) in problem structure
    if (resultStatus.ok() && resultStatus.value().has_primal_feasible_solution())
    {
        ProblemeAResoudre->ExistenceDUneSolution = OUI_PI;
        std::vector<double> primals;
        primals.reserve(ProblemeAResoudre->NombreDeVariables);
        std::vector<double> reducedCosts;
        reducedCosts.reserve(ProblemeAResoudre->NombreDeVariables);
        for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; ++i)
        {
            auto var = model.variable(i);
            primals.emplace_back(
              resultStatus.value().best_primal_solution().variable_values.at(var));
            reducedCosts.emplace_back(resultStatus.value().reduced_costs().at(var));
        }
        FillWithValues(ProblemeAResoudre->X, primals);
        FillWithValues(ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees, primals);
        FillWithValues(ProblemeAResoudre->CoutsReduits, reducedCosts);
        FillWithValues(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits, reducedCosts);

        // Dual values
        if (resultStatus.value().has_dual_feasible_solution())
        {
            std::vector<double> duals;
            duals.reserve(ProblemeAResoudre->NombreDeContraintes);
            for (int i = 0; i < ProblemeAResoudre->NombreDeContraintes; ++i)
            {
                duals.emplace_back(
                  resultStatus.value().dual_values().at(model.linear_constraint(i)));
            }
            FillWithValues(ProblemeAResoudre->CoutsMarginauxDesContraintes, duals);
            FillWithValues(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux, duals);
        }
    }
    else
    {
        Antares::logs.warning() << "Quadratic optimization failed: "
                                << resultStatus.status().ToString();
        ProblemeAResoudre->ExistenceDUneSolution = NON_PI;
        FillWithNaN(ProblemeAResoudre->X);
        FillWithNaN(ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees);
        FillWithNaN(ProblemeAResoudre->CoutsMarginauxDesContraintes);
        FillWithNaN(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux);
        FillWithNaN(ProblemeAResoudre->CoutsReduits);
        FillWithNaN(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits);
    }
}
