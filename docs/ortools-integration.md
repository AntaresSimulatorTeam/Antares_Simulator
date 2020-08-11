# Ortools integration

## Ortools avaialble solvers

### Integer optimisation
| Solver     | Licence |Website|
|:-------|-----|------|
| CLP_LINEAR_PROGRAMMING|Open source|https://github.com/coin-or/Clp|
| GLPK_LINEAR_PROGRAMMING|Open source|https://www.gnu.org/software/glpk/|
| GLOP_LINEAR_PROGRAMMING|Open source|https://developers.google.com/optimization/lp/glop|
| GUROBI_LINEAR_PROGRAMMING|Commercial|https://www.gurobi.com/|
| CPLEX_LINEAR_PROGRAMMING|Commercial|https://www.ibm.com/fr-fr/analytics/cplex-optimizer|
| XPRESS_LINEAR_PROGRAMMING|Commercial|https://www.fico.com/en/products/fico-xpress-solver|
| SIRIUS_LINEAR_PROGRAMMING|Open source|

### Mixed real integer optimisation
| Solver     | Licence |Website|
|:-------|-----|------|
| SCIP_MIXED_INTEGER_PROGRAMMING|Special license ZIB (research purposes as a member of a non-commercial or academic institution)|https://www.scipopt.org/|
| GLPK_MIXED_INTEGER_PROGRAMMING|Open sourcer|https://www.gnu.org/software/glpk/|
| CBC_MIXED_INTEGER_PROGRAMMING|Open source|https://github.com/coin-or/Cbc|
| GUROBI_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.gurobi.com/|
| CPLEX_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.ibm.com/fr-fr/analytics/cplex-optimizer|
| XPRESS_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.fico.com/en/products/fico-xpress-solver|
| SIRIUS_MIXED_INTEGER_PROGRAMMING|Open source|

### Constraint optimisation
| Solver     | Licence | Optimisation type |Website|
|:-------|-----|--------|------|
| BOP_INTEGER_PROGRAMMING|Open source|Boolean||
| SAT_INTEGER_PROGRAMMING|Open source|Boolean integer||

## Solver available for antares-solver
We need 2 types of solver :
* Integer
* Mixed real integer

Here are the solver used depending on antares-solver command line option ortools-solver:

|ortools-solver|Integer solver|Mixed real integer|
|:-------|-----|--------|
|sirius (default)|SIRIUS_LINEAR_PROGRAMMING|SIRIUS_MIXED_INTEGER_PROGRAMMING|
|coin|CLP_LINEAR_PROGRAMMING|CBC_MIXED_INTEGER_PROGRAMMING|

2 more solvers will be added when ortools is updated :

|ortools-solver|Integer solver|Mixed real integer|Waiting for|
|:-------|-----|--------|--------|
|xpress|XPRESS_LINEAR_PROGRAMMING|XPRESS_MIXED_INTEGER_PROGRAMMING| Update ortools building process for XPRESS support|
|glop-scip|GLOP_LINEAR_PROGRAMMING|SCIP_MIXED_INTEGER_PROGRAMMING|Merging of 7.8 ortools version in rte ortools fork|