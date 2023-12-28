#include <map>
#include <antares/array/matrix.h>

#include "constraints/constraint_builder_utils.h"
#include "opt_structure_probleme_a_resoudre.h"

/* 
   #########################################################
   ## This file contains a proof of concept for adding a  ##
   ## generic object into the weekly optimization problem ##
   #########################################################

The problem matrix has the following block structure (variables = columns, constraints = rows)
A B
C D

Where
- A represents existing variables & constraints
- B represents new variables in existing constraints
- C represents existing variables in new constraints
- D represents new variables in new constraints

This code assumes that B = 0 and C = 0, and only deals with D.
This means that there are no interactions between the existing variables and new variables, which not very realistic.

We don't want to change A (keep the problem as is)
We may want to change B, for example to include new contributions into the "Area Balance" constraint
We may want to change C, ie use existing variables in new constraints 

Changing B & C will require exposing additional information about existing variables & constraints

Since we can't really add new variables to an existing constraint, we may have to do the following
Create all variables (existing & new)
Create existing constraints, containing both existing & new variables
Create new constraints, containing both existing & new variables

The same follows for the objective function, we only consider new variables
*/

// Add a single variable, specifying bounds & cost
// Return index in the PROBLEME_ANTARES_A_RESOUDRE instance
int addSingleVariable(PROBLEME_ANTARES_A_RESOUDRE& problem,
                      double cost,
                      double xmin,
                      double xmax,
                      const std::string& label)
{
    const int newVarIndex = problem.NombreDeVariables;

    // Costs
    problem.CoutLineaire.push_back(cost);
    
    // Bounds
    problem.Xmin.push_back(xmin);
    problem.Xmax.push_back(xmax);
    // Metadata
    problem.NomDesVariables.push_back(label);

    problem.NombreDeVariables++;
    return newVarIndex;
}

// Add a single constraint, specifying coefficients, operator and RHS
// Return index in the PROBLEME_ANTARES_A_RESOUDRE instance
int addConstraint(PROBLEME_ANTARES_A_RESOUDRE& problem,
                  std::vector<double>& variableCoeffs,
                  std::vector<int>& variableIndices,
                  char op,
                  double rhs,
                  const std::string& label)
{
    const int newConstrIndex = problem.NombreDeContraintes;

    // RHS
    problem.SecondMembre.push_back(rhs);

    // Matrix & operator
    {
      int nbTerms = variableIndices.size();
      OPT_ChargerLaContrainteDansLaMatriceDesContraintes(&problem,
                                                         variableCoeffs,
                                                         variableIndices,
                                                         nbTerms,
                                                         op);
    }
    
    // Metadata
    problem.NomDesContraintes.push_back(label);

    return newConstrIndex;
}


using Matrix = Antares::Matrix<double>;
struct TimeSeriesStore
{
  std::map<std::string, ::Matrix> series;
};

using ID_Type = std::string;

struct HourlyVariable
{
    ID_Type label;

    ID_Type xmin;
    ID_Type xmax;
    ID_Type cost;

    int timestep;

    // TODO Move elsewhere
    // When using OR-Tools / MPSolver, replace int by MPVariable*
    int index;
};

struct Constraint
{
    ID_Type label;
    std::map<HourlyVariable*, double> variables;

    double rhs;
    char op;

    // TODO Move elsewhere
    // When using OR-Tools / MPSolver, replace int by MPConstraint*
    int index;
};

struct GenericObject
{
    std::vector<HourlyVariable> variables;
    std::vector<Constraint> constraints;
};

// TODO add scenario
void addObject(GenericObject& generic,
               TimeSeriesStore& store,
               PROBLEME_ANTARES_A_RESOUDRE& problem)
{
    for (auto& variable : generic.variables)
    {
        auto& cost = store.series[variable.cost];

        auto& xmin = store.series[variable.xmin];
        auto& xmax = store.series[variable.xmax];

        auto coeff = [&variable](::Matrix& matrix) {
            // Scenario here, instead of 0
            return matrix[0][variable.timestep];
        };

        int index = addSingleVariable(problem,
                                      coeff(cost),
                                      coeff(xmin),
                                      coeff(xmax),
                                      variable.label + std::to_string(variable.timestep));
        variable.index = index;
    }

    for (auto constraint : generic.constraints)
    {
        std::vector<int> variableIndices;
        std::vector<double> variableCoeffs;
        for (auto [var, coeff] : constraint.variables)
        {
            variableIndices.push_back(var->index);
            variableCoeffs.push_back(coeff);
        }

        int index = addConstraint(problem,
                                  variableCoeffs,
                                  variableIndices,
                                  constraint.op,
                                  constraint.rhs,
                                  constraint.label);

        constraint.index = index;
    }
}
               
