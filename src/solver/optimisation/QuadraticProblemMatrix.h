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
    QuadraticProblemMatrix(ConstraintBuilder& builder) :
     ProblemMatrixEssential(builder), exchangeBalanceGroup_(builder)
    {
        constraintgroups_ = {&exchangeBalanceGroup_};
    }

    void Run() override;

private:
    ExchangeBalanceGroup exchangeBalanceGroup_;
};