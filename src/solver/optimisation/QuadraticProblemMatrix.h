#pragma once

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "opt_fonctions.h"
#include "constraints/ExchangeBalanceGroup.h"
#include "ProblemMatrixEssential.h"

class QuadraticProblemMatrix : public ProblemMatrixEssential
{
public:
    QuadraticProblemMatrix(PROBLEME_HEBDO* problem_hebdo, ConstraintBuilder& builder) :
     ProblemMatrixEssential(problem_hebdo), exchangeBalanceGroup_(problem_hebdo, builder)
    {
        constraintgroups_ = {&exchangeBalanceGroup_};
    }

    void Run() override;

private:
    ExchangeBalanceGroup exchangeBalanceGroup_;
};