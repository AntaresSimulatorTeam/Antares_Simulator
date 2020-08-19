# Ortools integration

## Ortools available solvers
There is currently no ortools API to get a list of available solver.

We will be using name and enum defined in `linear_solver/linear_solver.cc`

### Integer optimisation
| Ortools solver name| Enum| Licence |Website|
|:-------|-------|-----|------|
| `clp`| CLP_LINEAR_PROGRAMMING|Open source|https://github.com/coin-or/Clp|
| `glpk_lp`| GLPK_LINEAR_PROGRAMMING|Open source|https://www.gnu.org/software/glpk/|
| `glop`| GLOP_LINEAR_PROGRAMMING|Open source|https://developers.google.com/optimization/lp/glop|
| `gurobi_lp`| GUROBI_LINEAR_PROGRAMMING|Commercial|https://www.gurobi.com/|
| `NOT_DEFINED`| CPLEX_LINEAR_PROGRAMMING|Commercial|https://www.ibm.com/fr-fr/analytics/cplex-optimizer|
| `xpress_lp`| XPRESS_LINEAR_PROGRAMMING|Commercial|https://www.fico.com/en/products/fico-xpress-solver|
| `NOT_DEFINED`| SIRIUS_LINEAR_PROGRAMMING|Open source|

### Mixed real integer optimisation
| Ortools solver name| Enum     | Licence |Website|
|:-------|-------|-----|------|
| `scip`| SCIP_MIXED_INTEGER_PROGRAMMING|Special license ZIB (research purposes as a member of a non-commercial or academic institution)|https://www.scipopt.org/|
| `glpk_mip`| GLPK_MIXED_INTEGER_PROGRAMMING|Open sourcer|https://www.gnu.org/software/glpk/|
| `cbc`| CBC_MIXED_INTEGER_PROGRAMMING|Open source|https://github.com/coin-or/Cbc|
| `gurobi_mip`| GUROBI_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.gurobi.com/|
| `NOT_DEFINED`| CPLEX_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.ibm.com/fr-fr/analytics/cplex-optimizer|
| `xpress_mip`| XPRESS_MIXED_INTEGER_PROGRAMMING|Commercial|https://www.fico.com/en/products/fico-xpress-solver|
| `NOT_DEFINED`| SIRIUS_MIXED_INTEGER_PROGRAMMING|Open source||

### Constraint optimisation
| Ortools solver name| Enum     | Licence | Optimisation type |Website|
|:-------|-------|-----|------|------|
| `bop`| BOP_INTEGER_PROGRAMMING|Open source|Boolean||
| `sat`| SAT_INTEGER_PROGRAMMING|Open source|Boolean integer||

## Solver available for antares-solver
We need 2 types of solver :
* Integer
* Mixed real integer

Here are the solver used depending on antares-solver command line option ortools-solver:

|ortools-solver|Integer solver|Mixed real integer|
|:-------|-----|--------|
|`sirius` (default)|SIRIUS_LINEAR_PROGRAMMING|SIRIUS_MIXED_INTEGER_PROGRAMMING|
|`coin`|CLP_LINEAR_PROGRAMMING|CBC_MIXED_INTEGER_PROGRAMMING|

5 more solvers will be added when ortools is updated :

|ortools-solver|Integer solver|Mixed real integer|Waiting for|
|:-------|-----|--------|--------|
|`xpress`|XPRESS_LINEAR_PROGRAMMING|XPRESS_MIXED_INTEGER_PROGRAMMING| Update ortools building process for XPRESS support|
|`glop-scip`|GLOP_LINEAR_PROGRAMMING|SCIP_MIXED_INTEGER_PROGRAMMING|Merging of 7.8 ortools version in rte ortools fork|
|`cplex`|CPLEX_LINEAR_PROGRAMMING|CPLEX_MIXED_INTEGER_PROGRAMMING| Update ortools building process for CPLEX support|
|`gurobi`|GUROBI_LINEAR_PROGRAMMING|GUROBI_MIXED_INTEGER_PROGRAMMING| Update ortools building process for gurobi support|
|`glpk`|GLPK_LINEAR_PROGRAMMING|GLPK_MIXED_INTEGER_PROGRAMMING| Update ortools building process for glpk support|